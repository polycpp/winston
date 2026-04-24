Quickstart
==========

This page walks through a minimal winston program end-to-end. Copy the
snippet, run it, then jump to :doc:`../tutorials/index` for task-oriented
walkthroughs or :doc:`../api/index` for the full reference.

We'll build a logger that writes JSON-formatted entries to both the
console and a file, attach structured metadata to each entry, and watch
level filtering drop anything below ``info``.

Full example
------------

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>

   using namespace polycpp;
   using namespace polycpp::winston;

   int main() {
       // Build a shared format pipeline:
       //   errors -> attach stack traces for exception objects
       //   timestamp -> add an ISO-8601 "timestamp" field
       //   json -> serialize everything to a single-line JSON record
       auto pipeline = combine({
           formats::errors({{"stack", JsonValue(true)}}),
           formats::timestamp(),
           formats::json(),
       });

       auto logger = createLogger({
           .level      = "info",
           .format     = pipeline,
           .transports = {
               std::make_shared<ConsoleTransport>(),
               std::make_shared<FileTransport>(
                   FileTransportOptions{.filename = "app.log"}),
           },
           .defaultMeta = {{"service", JsonValue(std::string("api"))}},
       });

       logger->info("server started", {{"port", JsonValue(3000)}});
       logger->warn("slow request",   {{"route", JsonValue(std::string("/users"))},
                                       {"ms",    JsonValue(412)}});
       logger->debug("this is dropped; level threshold is info");
       return 0;
   }

Compile it with the same CMake wiring from :doc:`installation`:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build
   ./build/my_app
   cat app.log

Expected output (one JSON object per line):

.. code-block:: text

   {"level":"info","message":"server started","port":3000,"service":"api","timestamp":"2026-04-23T10:00:00.000Z"}
   {"level":"warn","message":"slow request","ms":412,"route":"/users","service":"api","timestamp":"2026-04-23T10:00:00.001Z"}

What just happened
------------------

1. :cpp:func:`combine <polycpp::winston::combine>` chained three formats
   into a single pipeline. Each format transforms the
   :cpp:class:`LogInfo <polycpp::winston::LogInfo>` that flows through;
   the final ``formats::json`` finalising format serialises it into
   ``info.formattedMessage``, which is what the transports emit.

2. ``createLogger`` instantiated a :cpp:class:`Logger <polycpp::winston::Logger>`
   owning two transports.
   :cpp:class:`ConsoleTransport <polycpp::winston::ConsoleTransport>` writes
   to stdout;
   :cpp:class:`FileTransport <polycpp::winston::FileTransport>` appends
   to ``app.log``. Each can carry its own level threshold and format —
   see :doc:`../guides/per-transport-level`.

3. ``defaultMeta`` merged a ``service`` field into every entry. For
   request-scoped context — like a correlation ID that should span a
   single request — use :cpp:func:`Logger::child <polycpp::winston::Logger::child>`
   instead, which keeps the base logger untouched.

4. Level filtering is numeric: ``"info"`` is priority 2 under the npm
   preset, so ``debug`` (priority 5) is silently dropped before any
   format runs.

Next steps
----------

- :doc:`../tutorials/index` — step-by-step walkthroughs of common tasks.
- :doc:`../guides/index` — short how-tos for specific problems.
- :doc:`../api/index` — every public type, function, and option.
- :doc:`../examples/index` — runnable programs you can drop into a sandbox.
