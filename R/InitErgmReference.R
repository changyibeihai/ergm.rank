#  File R/InitErgmReference.R in package ergm.rank, part of the Statnet suite
#  of packages for network analysis, https://statnet.org .
#
#  This software is distributed under the GPL-3 license.  It is free,
#  open source, and has the attribution requirements (GPL Section 7) at
#  https://statnet.org/attribution
#
#  Copyright 2008-2019 Statnet Commons
#######################################################################
InitErgmReference.CompleteOrder <- function(lhs.nw, ...){
  list(name="CompleteOrder", init_methods = c("CD","zeros"))
}
