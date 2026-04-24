Write a custom format that redacts secrets
==========================================

Subclass :cpp:class:`Format <polycpp::winston::Format>` to rewrite
``LogInfo`` entries before they reach the finalising
:cpp:func:`formats::json <polycpp::winston::formats::json>`. This one
masks the ``password`` metadata key.

.. literalinclude:: ../../../examples/custom_format.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_WINSTON_BUILD_EXAMPLES=ON
   cmake --build build --target custom_format
   ./build/examples/custom_format
