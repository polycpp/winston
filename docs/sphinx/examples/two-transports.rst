Console plus rotating JSON file
===============================

Fan a single log call out to two transports:
:cpp:class:`ConsoleTransport <polycpp::winston::ConsoleTransport>`
with coloured ``level: message`` output, and
:cpp:class:`FileTransport <polycpp::winston::FileTransport>` with
JSON-per-line records for a log aggregator.

.. literalinclude:: ../../../examples/two_transports.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_WINSTON_BUILD_EXAMPLES=ON
   cmake --build build --target two_transports
   ./build/examples/two_transports
   cat app.log
