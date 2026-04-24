How do I drop specific entries in a format?
===========================================

A :cpp:class:`Format <polycpp::winston::Format>` returns
``std::optional<LogInfo>``. Return ``std::nullopt`` to drop the entry —
downstream formats and transports never see it.

.. code-block:: cpp

   #include <polycpp/winston/winston.hpp>
   using namespace polycpp;
   using namespace polycpp::winston;

   class DropHealthChecks : public Format {
   public:
       std::optional<LogInfo> transform(LogInfo info) override {
           if (info.get("route", JsonValue(std::string(""))).asString() == "/healthz") {
               return std::nullopt;        // drop
           }
           return info;
       }
   };

   auto logger = createLogger({
       .format = combine({
           std::make_shared<DropHealthChecks>(),
           formats::timestamp(),
           formats::json(),
       }),
       .transports = {std::make_shared<ConsoleTransport>()},
   });

``combine`` short-circuits on ``std::nullopt`` — the pipeline stops as
soon as any format drops the entry, so later formats don't waste work.

Same pattern works for redaction:

.. code-block:: cpp

   info.set("password", JsonValue(std::string("***")));
   return info;
