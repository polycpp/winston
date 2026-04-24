Format pipeline
===============

Every log entry flows through a
:cpp:class:`Format <polycpp::winston::Format>` — a transformer that
mutates the in-flight :cpp:class:`LogInfo <polycpp::winston::LogInfo>`,
or returns ``std::nullopt`` to drop it. Formats are chained with
:cpp:func:`combine <polycpp::winston::combine>`.

.. doxygenclass:: polycpp::winston::Format
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::CombinedFormat
   :members:
   :undoc-members:

.. doxygenfunction:: polycpp::winston::combine

.. doxygentypedef:: polycpp::winston::FormatOptions

Colorizer
---------

The colorizer is the one stateful format — it reads level→style name
pairs from the active :cpp:class:`LevelConfig <polycpp::winston::LevelConfig>`
(and the static ``Colorizer::addColors`` registry) and rewrites
``info.level`` / ``info.message`` with ANSI escapes.

.. doxygenclass:: polycpp::winston::Colorizer
   :members:
   :undoc-members:
