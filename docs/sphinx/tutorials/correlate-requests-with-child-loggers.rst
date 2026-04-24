Correlate HTTP requests with child loggers
==========================================

**You'll build:** an HTTP request handler that produces a child logger
per request — carrying a correlation ID, user ID, and route — and use
it to trace every log line back to the originating request. This is the
pattern you want the moment your logs land in a shared aggregator.

**You'll use:**
:cpp:func:`Logger::child <polycpp::winston::Logger::child>`,
:cpp:class:`ChildLogger <polycpp::winston::ChildLogger>`,
:cpp:class:`LoggerOptions::defaultMeta <polycpp::winston::LoggerOptions>`.

Step 1 — the base logger
------------------------

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp;
   using namespace polycpp::winston;

   auto base = createLogger({
       .level  = "info",
       .format = combine({formats::timestamp(), formats::json()}),
       .transports = {std::make_shared<ConsoleTransport>()},
       .defaultMeta = {{"service", JsonValue(std::string("api"))}},
   });

Every entry gains ``service: api`` and a timestamp. Good baseline for
a process-wide logger — but "I saw X during request Y" still needs
per-request metadata.

Step 2 — spawn a child per request
----------------------------------

Inside your request handler:

.. code-block:: cpp

   void handleRequest(const Request& req, Response& res) {
       auto log = base->child({
           {"request_id", JsonValue(req.id)},        // correlation ID
           {"route",      JsonValue(req.route)},
           {"method",     JsonValue(req.method)},
       });

       log.info("started");
       //  ... handler logic ...
       log.info("completed", {{"status", JsonValue(res.status)}});
   }

Every call on ``log`` inherits the base logger's level, transports, and
format — plus the three extra metadata keys. The base logger is
untouched, so concurrent requests don't stomp on each other's
correlation IDs.

Step 3 — nest child loggers for deeper context
----------------------------------------------

Children can spawn children. Use this for sub-operations that want
more context without restating the whole chain:

.. code-block:: cpp

   auto dbLog = log.child({{"component", JsonValue(std::string("db"))}});
   dbLog.info("executing query",
              {{"sql", JsonValue(std::string("SELECT ..."))}});

The resulting entry carries ``service``, ``request_id``, ``route``,
``method``, and ``component`` — all merged automatically.

Step 4 — keep the hot path cheap
--------------------------------

``child`` copies the default-meta object once; subsequent log calls
reuse it. If you're spawning thousands of children per second (think
per-event emission inside a tight loop), prefer passing the extra
metadata directly to a single ``log.info`` call — the child-logger
abstraction pays for itself when the same context covers dozens of
log lines.

Step 5 — middleware pattern
---------------------------

If your HTTP framework supports middleware, attach the child logger to
the request object and let handlers downstream retrieve it:

.. code-block:: cpp

   app.use([&](auto& req, auto& res, auto next) {
       req.log = base->child({
           {"request_id", JsonValue(req.id)},
           {"route",      JsonValue(req.route)},
       });
       req.log.info("request received");
       next();
       req.log.info("request finished", {{"status", JsonValue(res.status)}});
   });

From any handler: ``req.log.warn("cache miss", {...})`` and the
correlation ID rides along for free.

What you learned
----------------

- ``Logger::child`` is the canonical way to thread per-request context
  through downstream code without polluting the base logger.
- Children inherit level, format, and transports; they only *add*
  default metadata.
- Nest child loggers to layer context cheaply.
