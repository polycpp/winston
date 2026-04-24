Levels
======

:cpp:class:`polycpp::winston::LevelConfig` defines the name→priority
map and the colour names used by the
:cpp:func:`colorize <polycpp::winston::formats::colorize>` format. Three
presets ship out of the box — **npm**, **syslog**, and **cli** — but
you can build any custom map you like; the Logger only cares that
``threshold`` and ``message`` levels both resolve to numbers.

.. doxygenstruct:: polycpp::winston::LevelConfig
   :members:
   :undoc-members:

Using a preset
--------------

.. code-block:: cpp

   auto logger = createLogger({
       .level  = "debug",
       .levels = LevelConfig::syslog(),   // emerg < alert < crit < ... < debug
   });

Using custom levels
-------------------

.. code-block:: cpp

   LevelConfig lc;
   lc.levels = {{"fatal",0},{"error",1},{"audit",2},{"info",3},{"trace",4}};
   lc.colors = {{"audit","magenta"},{"trace","dim cyan"}};

   auto logger = createLogger({.level="info", .levels=std::move(lc)});
   logger->log("audit", "purchase placed");
