Profile expensive work with a Profiler
======================================

**You'll build:** a batch job that uses
:cpp:class:`Profiler <polycpp::winston::Profiler>` to measure the three
phases of an ETL pipeline and emit a single timing line per phase —
without scattering ``chrono`` calls across the handler.

**You'll use:**
:cpp:func:`Logger::startTimer <polycpp::winston::Logger::startTimer>`,
:cpp:func:`Logger::profile <polycpp::winston::Logger::profile>`.

Step 1 — scoped start/stop
--------------------------

``startTimer`` returns a :cpp:class:`Profiler <polycpp::winston::Profiler>`
that captures *now*. Call ``done`` to emit an ``info``-level log line
with a ``durationMs`` field:

.. code-block:: cpp

   auto timer = logger->startTimer();
   runExtract();
   timer.done("extract complete", {{"rows", polycpp::JsonValue(1'432'511)}});
   // -> {"level":"info","message":"extract complete","durationMs":1842,"rows":1432511}

The Profiler holds a weak reference to the logger — if the logger goes
away, ``done`` is a no-op.

Step 2 — toggle-based profiling for repeated identifiers
--------------------------------------------------------

When the *same* identifier needs to bracket code in two different
functions, use ``profile(id)``. The first call arms the timer; the
second call emits it:

.. code-block:: cpp

   logger->profile("transform");
   runTransform();
   logger->profile("transform", {{"rows", polycpp::JsonValue(transformed)}});

Any subsequent ``profile("transform")`` pair restarts the timer — the
Logger keeps a map of armed identifiers.

Step 3 — structured ETL timings
-------------------------------

Putting it all together:

.. code-block:: cpp

   void runETL(std::shared_ptr<Logger> logger) {
       auto total = logger->startTimer();

       {
           auto extract = logger->startTimer();
           auto rows = runExtract();
           extract.done("extract", {{"rows", polycpp::JsonValue(rows)}});
       }

       logger->profile("transform");
       auto transformed = runTransform();
       logger->profile("transform", {{"rows", polycpp::JsonValue(transformed)}});

       {
           auto load = logger->startTimer();
           runLoad();
           load.done("load complete");
       }

       total.done("ETL complete");
   }

Each ``done`` call emits exactly one log line, so your dashboard can
group by ``message`` to see the distribution of each phase.

Step 4 — pair with a logstash/json format
-----------------------------------------

Because the timing field is plain metadata, any finalising format
picks it up. With ``formats::json()``, each record lands in a
log aggregator ready for querying; with
:cpp:func:`formats::logstash <polycpp::winston::formats::logstash>`,
you get the ``@timestamp``-shaped Logstash record Elasticsearch likes.

What you learned
----------------

- ``startTimer`` is the RAII-style API; ``profile(id)`` is the
  toggle-pair API. Pick whichever fits the control flow.
- Timings land in ``durationMs`` metadata — so any finalising format
  picks them up automatically.
- The profiler uses the logger it was started from; the entry is
  emitted at the logger's current level, not a fixed one.
