Built-in formats
================

The ``polycpp::winston::formats`` namespace holds the 16 finishing and
transforming formats that ship with the library. They're ordinary
factory functions — call one, pass the result to
:cpp:func:`combine <polycpp::winston::combine>`, hand the combined
format to ``LoggerOptions::format`` or ``TransportOptions::format``.

Finalising formats
------------------

These turn ``info.formattedMessage`` into the string a transport
writes. Include exactly one in each chain, usually last.

.. doxygenfunction:: polycpp::winston::formats::json
.. doxygenfunction:: polycpp::winston::formats::simple
.. doxygenfunction:: polycpp::winston::formats::printf
.. doxygenfunction:: polycpp::winston::formats::logstash
.. doxygenfunction:: polycpp::winston::formats::cli
.. doxygenfunction:: polycpp::winston::formats::prettyPrint

Enrichment
----------

Add fields to ``info.metadata`` or mutate ``info.message`` without
finalising.

.. doxygenfunction:: polycpp::winston::formats::timestamp
.. doxygenfunction:: polycpp::winston::formats::label
.. doxygenfunction:: polycpp::winston::formats::ms
.. doxygenfunction:: polycpp::winston::formats::metadata
.. doxygenfunction:: polycpp::winston::formats::errors
.. doxygenfunction:: polycpp::winston::formats::splat

Layout
------

Cosmetics for human-readable output.

.. doxygenfunction:: polycpp::winston::formats::align
.. doxygenfunction:: polycpp::winston::formats::padLevels
.. doxygenfunction:: polycpp::winston::formats::colorize
.. doxygenfunction:: polycpp::winston::formats::uncolorize

Options
-------

.. doxygenstruct:: polycpp::winston::formats::TimestampOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::LabelOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::ErrorsOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::MetadataOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::ColorizeOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::UncolorizeOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::PadLevelsOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::PrettyPrintOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::JsonFormatOptions
   :members:
   :undoc-members:

.. doxygenstruct:: polycpp::winston::formats::CliOptions
   :members:
   :undoc-members:
