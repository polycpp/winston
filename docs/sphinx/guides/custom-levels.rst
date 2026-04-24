How do I define custom log levels?
==================================

Build a :cpp:class:`LevelConfig <polycpp::winston::LevelConfig>` and
pass it as ``LoggerOptions::levels``:

.. code-block:: cpp

   LevelConfig lc;
   lc.levels = {
       {"fatal", 0},
       {"error", 1},
       {"audit", 2},
       {"info",  3},
       {"trace", 4},
   };
   lc.colors = {
       {"audit", "magenta"},
       {"trace", "dim cyan"},
   };

   auto logger = createLogger({
       .level  = "info",
       .levels = std::move(lc),
       .format = combine({formats::colorize(), formats::simple()}),
       .transports = {std::make_shared<ConsoleTransport>()},
   });

   logger->log("audit", "purchase placed");   // emitted
   logger->log("trace", "edge detail");       // dropped — above info

Lower numeric priority = more severe (matching npm/syslog). Colour
names are space-separated styles compatible with
``polycpp::util::styleText`` — ``"bold red"``, ``"dim cyan"``,
``"inverse green"`` all work.

Pass :cpp:func:`LevelConfig::syslog <polycpp::winston::LevelConfig::syslog>`
or :cpp:func:`LevelConfig::cli <polycpp::winston::LevelConfig::cli>` for
the ready-made presets.
