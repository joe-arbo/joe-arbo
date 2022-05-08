--
-- Lua script to be used with wrk for CS 3214 benchmarking.
--
-- @author godmar, CS 3214 Spring 2016
--
--

-- From: http://regex.info/blog/lua/json

luajson = os.getenv("JSON_LUA")
JSON = (loadfile (luajson))() -- one-time load of the routines

function prepare_stat(stat)
   pc = { }
   for _, p in pairs({ 50, 90, 95, 99, 99.999 }) do
      pc[tostring(p)] = stat:percentile(p)
   end
   return {
       min = stat.min,
       max = stat.max,
       mean = stat.mean,
       stdev = stat.stdev,
       percentiles = pc
   }
end

done = function(summary, latency, requests)
   -- we would like to include other parameters here, such as
   -- the number of threads or connections per thread, but
   -- these do not appear to be accessible at this point.
   result = {
       summary = summary,
       latency = prepare_stat(latency),
       requests = prepare_stat(requests)
   }
   ofile = os.getenv("JSON_OUTPUT_FILE")
   print (string.format("Writing results to: %s", ofile))
   local f = assert(io.open(ofile, "w"))
   f:write(JSON:encode_pretty(result))
   f:close()
end
