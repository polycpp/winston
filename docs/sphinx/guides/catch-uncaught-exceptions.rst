How do I log uncaught exceptions automatically?
===============================================

Add a transport with ``handleExceptions = true`` and set the logger's
``handleExceptions`` flag. The library wires the transport into the
:cpp:class:`ExceptionHandler <polycpp::winston::ExceptionHandler>`,
which captures ``std::terminate`` / ``std::set_terminate`` handlers on
your behalf and logs the failure before the process exits.

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp::winston;

   FileTransportOptions fopts;
   fopts.filename         = "uncaught.log";
   fopts.handleExceptions = true;
   auto uncaught = std::make_shared<FileTransport>(std::move(fopts));

   auto logger = createLogger({
       .level  = "info",
       .format = combine({formats::errors({{"stack", polycpp::JsonValue(true)}}),
                          formats::timestamp(),
                          formats::json()}),
       .transports = {std::make_shared<ConsoleTransport>(), uncaught},
       .handleExceptions = true,
   });

Pair :cpp:func:`formats::errors <polycpp::winston::formats::errors>`
with ``stack=true`` so the exception's stack trace lands in the log
record.

The default exit behaviour is ``exitOnError = true``. Set it to a
predicate to filter which failures should terminate the process:

.. code-block:: cpp

   LoggerOptions opts;
   opts.exitOnError = [](const polycpp::Error& e) {
       return e.code() != "ERR_RECOVERABLE";
   };
