(unwatch all)
(clear)
(dribble-on "Actual//stobjcst.out")
(batch "stobjcst.bat")
(dribble-off)
(clear)
(open "Results//stobjcst.rsl" stobjcst "w")
(load "compline.clp")
(printout stobjcst "stobjcst.clp differences are as follows:" crlf)
(compare-files "Expected//stobjcst.out" "Actual//stobjcst.out" stobjcst)
(close stobjcst)
