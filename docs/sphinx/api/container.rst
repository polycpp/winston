Container
=========

The :cpp:class:`polycpp::winston::Container` is a registry of named
loggers. One :cpp:func:`loggers() <polycpp::winston::loggers>` singleton
ships out of the box — register a logger in ``main``, then call
``winston::loggers().get("payments")`` from anywhere to reach the same
instance.

.. doxygenclass:: polycpp::winston::Container
   :members:
   :undoc-members:
