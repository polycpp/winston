Ship logs to multiple transports
================================

**You'll build:** a Logger that writes human-readable coloured output to
the terminal, structured JSON to a rotating log file for your logging
pipeline, and only errors to a dedicated ``stderr`` stream — all from a
single call to :cpp:func:`createLogger <polycpp::winston::createLogger>`.

**You'll use:**
:cpp:func:`createLogger <polycpp::winston::createLogger>`,
:cpp:class:`ConsoleTransport <polycpp::winston::ConsoleTransport>`,
:cpp:class:`FileTransport <polycpp::winston::FileTransport>`,
:cpp:class:`StreamTransport <polycpp::winston::StreamTransport>`,
:cpp:func:`combine <polycpp::winston::combine>`, and the built-in
formats :cpp:func:`timestamp <polycpp::winston::formats::timestamp>`,
:cpp:func:`colorize <polycpp::winston::formats::colorize>`,
:cpp:func:`simple <polycpp::winston::formats::simple>`,
:cpp:func:`json <polycpp::winston::formats::json>`.

Step 1 — terminal: coloured, human-readable
-------------------------------------------

Combine ``colorize`` (rewrite ``info.level`` with ANSI escapes) with
``simple`` (produce ``"level: message { meta }"``):

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp::winston;

   auto consoleFormat = combine({
       formats::colorize(),
       formats::simple(),
   });

   auto console = std::make_shared<ConsoleTransport>(ConsoleTransportOptions{
       .level  = "info",
       .format = consoleFormat,
   });

``simple`` is *finalising* — it sets ``info.formattedMessage``, which is
what the transport writes. Keep that in mind when composing chains:
exactly one finalising format per chain, usually last.

Step 2 — file: structured JSON with rotation
--------------------------------------------

Every production pipeline you'll wire into (Loki, Elasticsearch,
CloudWatch) wants JSON. Add a timestamp and a service name while
we're at it:

.. code-block:: cpp

   auto fileFormat = combine({
       formats::timestamp(),
       formats::errors({{"stack", polycpp::JsonValue(true)}}),
       formats::json(),
   });

   FileTransportOptions fopts;
   fopts.filename     = "app.log";
   fopts.maxsize      = 10 * 1024 * 1024;       // rotate at 10 MiB
   fopts.maxFiles     = 5;                      // keep 5 rotated files
   fopts.tailable     = true;                   // app.log always points at the newest
   fopts.zippedArchive = true;                  // gzip the rotated files

   auto file = std::make_shared<FileTransport>(std::move(fopts));
   file->format = fileFormat;

Step 3 — stderr: errors only, unformatted
-----------------------------------------

Processes that run under systemd or Docker benefit from *separate*
stderr — your orchestrator tags it differently. Use a StreamTransport
over ``std::cerr`` with a level threshold of ``error``:

.. code-block:: cpp

   StreamTransportOptions sopts;
   sopts.level = "error";
   auto err = std::make_shared<StreamTransport>(std::cerr, std::move(sopts));
   err->format = combine({formats::timestamp(), formats::simple()});

Step 4 — wire them together
---------------------------

.. code-block:: cpp

   auto logger = createLogger({
       .level      = "info",
       .transports = {console, file, err},
       .defaultMeta = {{"service", polycpp::JsonValue(std::string("api"))}},
   });

   logger->info("server started", {{"port", polycpp::JsonValue(3000)}});
   logger->warn("slow request",   {{"ms",   polycpp::JsonValue(412)}});
   logger->error("database offline");

Behind the scenes, each entry:

1. Passes the logger-wide level filter (``info`` and above).
2. Merges ``defaultMeta`` into its metadata.
3. Runs the (empty) logger-wide format pipeline.
4. Fans out to each transport, which runs its own level filter, then
   its per-transport format, then writes.

So ``debug`` is dropped up front; ``error`` reaches all three
transports; ``warn`` reaches console + file but not stderr.

Step 5 — close cleanly on shutdown
----------------------------------

Make sure buffered file I/O is flushed before ``main`` returns:

.. code-block:: cpp

   logger->close();

``close`` emits the ``"close"`` event on each transport — subscribe if
you need a signal from a teardown hook.

What you learned
----------------

- Transports get their own level and format. The logger-wide ones
  apply first; per-transport ones apply after.
- Finalising formats (``json``, ``simple``, ``printf``, ``logstash``,
  ``cli``, ``prettyPrint``) set ``info.formattedMessage`` — one per
  pipeline, please.
- ``FileTransport`` handles rotation, ``ConsoleTransport`` handles
  colour, ``StreamTransport`` adapts anything with an
  ``std::ostream&``.
