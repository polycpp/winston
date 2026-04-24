Default logger
==============

A process-wide default logger is available out of the box. Reconfigure
it via :cpp:func:`winston::configure <polycpp::winston::configure>`,
attach transports with
:cpp:func:`winston::add <polycpp::winston::add>`, or call the
top-level logging functions that forward to it.

Factory and configuration
-------------------------

.. doxygenfunction:: polycpp::winston::createLogger
.. doxygenfunction:: polycpp::winston::defaultLogger
.. doxygenfunction:: polycpp::winston::configure
.. doxygenfunction:: polycpp::winston::version

Top-level logging
-----------------

.. doxygenfunction:: polycpp::winston::log(const std::string&, const std::string&)
.. doxygenfunction:: polycpp::winston::log(const std::string&, const std::string&, const JsonObject&)
.. doxygenfunction:: polycpp::winston::log(LogInfo)
.. doxygenfunction:: polycpp::winston::error
.. doxygenfunction:: polycpp::winston::warn
.. doxygenfunction:: polycpp::winston::info
.. doxygenfunction:: polycpp::winston::http
.. doxygenfunction:: polycpp::winston::verbose
.. doxygenfunction:: polycpp::winston::debug
.. doxygenfunction:: polycpp::winston::silly

Transport and metadata helpers
------------------------------

.. doxygenfunction:: polycpp::winston::add
.. doxygenfunction:: polycpp::winston::remove
.. doxygenfunction:: polycpp::winston::clear
.. doxygenfunction:: polycpp::winston::child
.. doxygenfunction:: polycpp::winston::profile
.. doxygenfunction:: polycpp::winston::startTimer
.. doxygenfunction:: polycpp::winston::loggers
.. doxygenfunction:: polycpp::winston::addColors
