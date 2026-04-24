How do I reach the same logger from different modules?
======================================================

Register it in the default
:cpp:class:`Container <polycpp::winston::Container>` and pull it out by
name:

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp::winston;

   // In main or your wiring module:
   loggers().add("payments", LoggerOptions{
       .level  = "info",
       .format = combine({formats::timestamp(), formats::json()}),
       .transports = {std::make_shared<ConsoleTransport>()},
   });

   // Anywhere else in the process:
   loggers().get("payments").info("refund issued",
       {{"amount", polycpp::JsonValue(12.50)}});

``loggers().get()`` returns a reference to the shared logger; every
caller logs through the same transports and format pipeline.

``loggers().has(name)`` checks before ``get``. Call
``loggers().close("payments")`` to flush buffered writes and remove the
logger — the container's
:cpp:func:`close <polycpp::winston::Container::close>` variant with no
argument closes all registered loggers.
