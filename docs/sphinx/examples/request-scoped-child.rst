Request-scoped child loggers
============================

Spawn a :cpp:class:`ChildLogger <polycpp::winston::ChildLogger>` per
HTTP request so every log line carries the correlation ID, route, and
method — without polluting the base logger or the handler signatures.

.. literalinclude:: ../../../examples/request_scoped_child.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_WINSTON_BUILD_EXAMPLES=ON
   cmake --build build --target request_scoped_child
   ./build/examples/request_scoped_child
