Logger
======

:cpp:class:`polycpp::winston::Logger` is the central orchestrator. It
owns a :cpp:class:`LevelConfig <polycpp::winston::LevelConfig>`, an
optional logger-wide :cpp:class:`Format <polycpp::winston::Format>`,
and a vector of :cpp:class:`Transport <polycpp::winston::Transport>`
instances — and runs level filtering, metadata merging, format
dispatch, and per-transport fan-out on every call.

.. doxygenstruct:: polycpp::winston::LoggerOptions
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::Logger
   :members:
   :undoc-members:

Profiler and child loggers
--------------------------

.. doxygenclass:: polycpp::winston::Profiler
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::ChildLogger
   :members:
   :undoc-members:

Exception handler
-----------------

.. doxygenclass:: polycpp::winston::ExceptionHandler
   :members:
   :undoc-members:
