winston
=======

**Structured logging with composable formats and transports**

Build a Logger once, wire any combination of transports (console, file, raw stream) and formats (json, simple, printf, colorize, timestamp, label, ms, metadata, splat, logstash, ...) into it, and log with typed metadata. API shape and option names mirror npm winston one-to-one — including child loggers, profilers, the default logger, and the named-logger container.

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp;
   using namespace polycpp::winston;

   auto logger = createLogger({
       .level      = "info",
       .format     = combine({timestamp(), json()}),
       .transports = {
           std::make_shared<ConsoleTransport>(),
           std::make_shared<FileTransport>(FileTransportOptions{.filename = "app.log"}),
       },
   });

   logger->info("server started", {{"port", JsonValue(3000)}});
   logger->warn("slow request",   {{"route", JsonValue(std::string("/api"))}, {"ms", JsonValue(412)}});

.. grid:: 2

   .. grid-item-card:: Drop-in familiarity
      :margin: 1

      Mirrors the Node.js winston package API (createLogger, format.*, transport.Console/File/Stream, levels, child loggers) one-to-one.

   .. grid-item-card:: C++20 native
      :margin: 1

      Header-only where possible, zero-overhead abstractions, ``constexpr``
      and ``std::string_view`` throughout.

   .. grid-item-card:: Tested
      :margin: 1

      Fourteen Google Test suites covering the logger, level config, every built-in format, all three transports, containers, exception handlers, and the default logger surface.

   .. grid-item-card:: Plays well with polycpp
      :margin: 1

      Uses the same JSON value, error, and typed-event types as the rest of
      the polycpp ecosystem — no impedance mismatch.

Getting started
---------------

.. code-block:: bash

   # With FetchContent (recommended)
   FetchContent_Declare(
       polycpp_winston
       GIT_REPOSITORY https://github.com/polycpp/winston.git
       GIT_TAG        master
   )
   FetchContent_MakeAvailable(polycpp_winston)
   target_link_libraries(my_app PRIVATE polycpp::winston)

:doc:`Installation <getting-started/installation>` · :doc:`Quickstart <getting-started/quickstart>` · :doc:`Tutorials <tutorials/index>` · :doc:`API reference <api/index>`

.. toctree::
   :hidden:
   :caption: Getting started

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :hidden:
   :caption: Tutorials

   tutorials/index

.. toctree::
   :hidden:
   :caption: How-to guides

   guides/index

.. toctree::
   :hidden:
   :caption: API reference

   api/index

.. toctree::
   :hidden:
   :caption: Examples

   examples/index
