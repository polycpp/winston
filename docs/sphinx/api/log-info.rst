LogInfo
=======

Every entry that flows through the format pipeline is a
:cpp:class:`polycpp::winston::LogInfo` — a struct of level, message,
splat (printf arguments), and an open-ended ``metadata`` map. Custom
formats mutate it in place; finalising formats write their output into
``formattedMessage``.

.. doxygenstruct:: polycpp::winston::LogInfo
   :members:
   :undoc-members:
