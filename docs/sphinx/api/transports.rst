Transports
==========

A :cpp:class:`Transport <polycpp::winston::Transport>` is the sink at
the end of the pipeline. Each one carries its own level threshold and
optional per-transport format, so you can send ``debug`` to a file
while keeping the console at ``warn``.

Base class
----------

.. doxygenstruct:: polycpp::winston::TransportOptions
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::Transport
   :members:
   :undoc-members:

ConsoleTransport
----------------

.. doxygenstruct:: polycpp::winston::ConsoleTransportOptions
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::ConsoleTransport
   :members:
   :undoc-members:

FileTransport
-------------

File transport with size-based rotation, optional gzip compression, and
a tailable mode that keeps the base filename pointing at the most recent
data.

.. doxygenstruct:: polycpp::winston::FileTransportOptions
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::FileTransport
   :members:
   :undoc-members:

StreamTransport
---------------

Any ``std::ostream`` you already own — ``std::cerr``, a memory buffer
for tests, a custom sink that writes to syslog.

.. doxygenstruct:: polycpp::winston::StreamTransportOptions
   :members:
   :undoc-members:

.. doxygenclass:: polycpp::winston::StreamTransport
   :members:
   :undoc-members:
