How do I set a different level threshold per transport?
========================================================

Set ``level`` on the transport's options struct — the Logger still
filters first, then each transport filters its own stream:

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp::winston;

   auto console = std::make_shared<ConsoleTransport>(ConsoleTransportOptions{
       .level = "warn",            // console only gets warn+
   });

   FileTransportOptions fopts;
   fopts.filename = "app.log";
   fopts.level    = "debug";       // file captures debug+
   auto file = std::make_shared<FileTransport>(std::move(fopts));

   auto logger = createLogger({
       .level      = "debug",      // logger admits debug+
       .transports = {console, file},
   });

   logger->debug("hi");            // file only
   logger->warn("slow");           // both

The :cpp:class:`Logger <polycpp::winston::Logger>`'s level is the outer
filter — raise it past a transport threshold and that transport
silently stops receiving entries even if its own threshold is more
permissive.
