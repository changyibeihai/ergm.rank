ergm.etamap <- function(model) {
# Take a model object and create a 'mapping' from the parameter of
# interest, theta, to the canonical parameter, eta.  This 'mapping'
# is a list with three items:
#  The first item, canonical, summarizes the
# one-to-one correspondence between those components of theta that
# are actually canonical and their counterparts in eta.
#  The second item,
# curved, is itself a list, with one element per curved EF term in the
# model.
#  The third item, etalength, is the length of eta.
# The function ergm.eta actually carries out the mapping.
  etamap <- list(canonical = NULL, curved=list(), etalength=NULL)
  from <- 1
  to <- 1
  a <- 1
  for (i in 1:length(model$terms)) {
    j <- model$terms[[i]]$inputs[2]
    mti <- model$terms[[i]]
    if (is.null(mti$params)) { # Not a curved parameter
      etamap$canonical <- c(etamap$canonical, to:(to+j-1))
      from <- from+j
      to <- to+j
    } else { # curved parameter
      k <- length(mti$params)
      etamap$canonical <- c(etamap$canonical, rep(0, k))
      etamap$curved[[a]] <- list(from=from:(from+k-1),
                                 to=to:(to+j-1),
                                 map=mti$map, gradient=mti$gradient,
                                 cov=mti$eta.cov)  #Added by CTB 1/28/06
      from <- from+k
      to <- to+j
      a <- a+1
    }
  }
  etamap$etalength <- to-1
  etamap
} 
