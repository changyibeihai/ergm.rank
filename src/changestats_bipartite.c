#include "changestats_bipartite.h"

void d_bimix (int ntoggles, Vertex *heads, Vertex *tails,
		              ModelTerm *mtp, Network *nwp) {

  int matchvalh, matchvalt;
  Vertex h, t, nnodes, nstats;
  int i, j, edgeflag=0;

  nstats = mtp->nstats;
  nnodes = (mtp->ninputparams)-2*nstats;
//  Rprintf("ninput %d nstats %d\n", mtp->ninputparams, mtp->nstats);
//  Rprintf("nodes %d\n", nnodes);
// for (h=0; h<nnodes; h++){
//   Rprintf("match h %d att h %f\n", 
//	    h, mtp->attrib[h+nstats]);
// }
// for (h=0; h<nstats; h++){
//   Rprintf("match h %d in h %f in t %f\n", 
//	    h, mtp->inputparams[h], mtp->inputparams[h+nstats]);
// }
  for (i=0; i < nstats; i++) 
    mtp->dstats[i] = 0.0;

  for (i=0; i<ntoggles; i++) 
    {
      h=heads[i];
      t=tails[i];
      matchvalh = mtp->attrib[h-1+nstats];
      matchvalt = mtp->attrib[t-1+nstats];
//     if(matchvalt < matchvalh){
//       matchswap = matchvalh;
//       matchvalh = matchvalt;
//       matchvalt = matchswap;
//      }       
      edgeflag=(EdgetreeSearch(h, t, nwp->outedges) != 0);
      for (j=0; j<nstats; j++) 
	  {
           if(matchvalh==mtp->inputparams[nstats+j] &&
	      matchvalt==mtp->inputparams[       j]
	     ){mtp->dstats[j] += edgeflag ? -1.0 : 1.0;}
	  }
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }
  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

/*****************
 void d_adegree

*****************/
void d_adegree (int ntoggles, Vertex *heads, Vertex *tails, 
	        ModelTerm *mtp, Network *nwp) 
{
  int i, j, echange;
  Vertex h, t, hd, deg, *id, *od;
  TreeNode *oe;  

  oe=nwp->outedges;
  id=nwp->indegree;
  od=nwp->outdegree;
  for (i=0; i < mtp->nstats; i++) 
    mtp->dstats[i] = 0.0;
  
  for (i=0; i<ntoggles; i++)
    {      
      echange = (EdgetreeSearch(h=heads[i], t=tails[i], oe) == 0) ? 1 : -1;
      hd = od[h] + id[h];
//   Rprintf("h %d t %d hd %d\n", h, t, hd);
      for(j = 0; j < mtp->nstats; j++) 
	{
	  deg = (Vertex)mtp->inputparams[j];
          mtp->dstats[j] += (hd + echange == deg) - (hd == deg);
	}
      
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }

  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}
/*****************
 void d_edegree

*****************/
void d_edegree (int ntoggles, Vertex *heads, Vertex *tails, 
	        ModelTerm *mtp, Network *nwp) 
{
  int i, j, echange;
  Vertex h, t, td=0, deg, *id, *od;
  TreeNode *oe;  

  oe=nwp->outedges;
  id=nwp->indegree;
  od=nwp->outdegree;
  for (i=0; i < mtp->nstats; i++) 
    mtp->dstats[i] = 0.0;
  
  for (i=0; i<ntoggles; i++)
    {      
      echange = (EdgetreeSearch(h=heads[i], t=tails[i], oe) == 0) ? 1 : -1;
      td = od[t] + id[t];
//   Rprintf("h %d t %d td %d\n", h, t, td);
      for(j = 0; j < mtp->nstats; j++) 
	{
	  deg = (Vertex)mtp->inputparams[j];
          mtp->dstats[j] += (td + echange == deg) - (td == deg);
	}
      
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }

  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

void d_biduration (int ntoggles, Vertex *heads, Vertex *tails,
		   ModelTerm *mtp, Network *nwp)
{
  Vertex h, t, hh, ht;
  int i, k, nprevedge, edgeflag, discord, lookmore;
  Vertex nevents, nactors;
  double change=0.0;

  nprevedge = (int)((mtp->inputparams[0]));
  nactors = (int)((mtp->inputparams[1]));
//  nactors = nwp->bipartite;
//  nevents = nwp->nnodes - nwp->bipartite;
  nevents = nwp->nnodes - nactors;

//  Rprintf("nprevedge %d\n", nprevedge);
//  Rprintf("nwp->bipartite %d nevents %f\n", nwp->bipartite, nwp->bipartite);
//  Rprintf("nactors %d nevents %d\n", nactors, nevents);
//  for (k=1; k<=nprevedge; k++) {
//      Rprintf("k %d x0.h %d x0.t %d\n", k,
//	      (Vertex)(mtp->attrib[          k]),
//	      (Vertex)(mtp->attrib[nprevedge+k]));
//  }

  *(mtp->dstats) = 0.0;

  for (i=0; i<ntoggles; i++) 
    {
      /*Get the initial state of the edge and its reflection*/
      edgeflag=(EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
//      if(h > t){
//	hh = t;
//	ht = h;
//      }else{
//	hh = h;
//	ht = t;
//      }
      // 1 = edge removed -1 = edge added
      discord = edgeflag ? 1 : -1;

// Rprintf("nprevedge %d\n", nprevedge);

      k=1;
      lookmore=1;
      while(lookmore && k <= nprevedge){
//       Rprintf("h %d t %d hh %d ht %d\n",
//	      h,t,hh,ht);
//       Rprintf("h %d t %d hh %d ht %d\n",
//	      ht,hh,(Vertex)(mtp->attrib[            k]),
//	            (Vertex)(mtp->attrib[nprevedge + k]));
       if(ht == (Vertex)(mtp->attrib[            k]) &&
          hh == (Vertex)(mtp->attrib[nprevedge + k])
	 ){
//       Rprintf("h %d t %d hh %d ht %d\n",
//	      h,t,(Vertex)(mtp->attrib[            k]),
//	          (Vertex)(mtp->attrib[nprevedge + k]));

           /*If the proposed edge existed in x0, get dissolution rate */
	   /* lookmore is location of (hh, ht) in the dissolve matrix */
//           lookmore=2*nprevedge+(hh-1)+(ht-1-nevents)*nactors+1;
           lookmore=2*nprevedge+(hh-1)+(ht-1-nactors)*nactors+1;
	   /* change is NEGATIVE the dissolve rate if an edge exists */
//       Rprintf("hh %d ht %d lookmore %d att %f\n",hh, ht, lookmore, (double)(mtp->attrib[lookmore]));
//           change=-discord*(double)(mtp->attrib[lookmore]);
           change=-discord;
//       Rprintf("lookmore %d change %f discord %d \n",lookmore, change, discord);
	   lookmore=0;
           /*Update the change statistics, as appropriate*/
	   /*change is the number of edges dissolved*/
           *(mtp->dstats) += (double)change;
//      if(change!=0){
//       Rprintf("h %d t %d hh %d ht %d edgeflag %d beta %f\n",
//	      h,t,hh,ht, edgeflag, (double)(mtp->attrib[lookmore]));
//       Rprintf("edgeflag %d change %f discord %d\n",
//	      edgeflag, change, discord);
//      }
          }else{
		   ++k;
	  }
      }

      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }
  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

//// *****************
//// void d_actor
////
//// *****************
//
//void d_actor (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  int i, j, echange;
//  Vertex h, t, deg, nevents, nactors;
//  
//  nactors = (Vertex)inp->inputparams[0];
//  nevents = (nwp->nnodes) - nactors;
////  Rprintf("nevents %d\n", nevents);
////  Rprintf("nactors %d\n", nactors);
////  Rprintf("inp->nstats %d\n", inp->nstats);
////  Rprintf("ntggles %d\n", ntoggles);
//
//  for (i=0; i < inp->nstats; i++) 
//    inp->dstats[i] = 0.0;
//  
//  for (i=0; i<ntoggles; i++)
//    {      
//      echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
//      j=0;
//      deg = (Vertex)inp->inputparams[j+1];
//      while(deg != h && j < inp->nstats){
//       j++;
//       deg = (Vertex)inp->inputparams[j+1];
//      }
////  Rprintf("deg %d j %d\n", deg, j);
//      if(j < inp->nstats){inp->dstats[j] += echange;}
////  Rprintf("heads[i] %d tails[i] %d echange %d j %d deg %d\n", heads[i], tails[i], echange,j,inp->dstats[j]);
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_event (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  int i, j, echange;
//  Vertex h, t, deg, nevents, nactors;
//  
//  nactors = (Vertex)inp->inputparams[0];
//  nevents = (nwp->nnodes) - nactors;
////  Rprintf("nevents %d\n", nevents);
////  Rprintf("nactors %d\n", nactors);
//  
//  for (i=0; i < inp->nstats; i++) 
//    inp->dstats[i] = 0.0;
//  
//  for (i=0; i<ntoggles; i++)
//    {      
//      echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
//      j=0;
//      deg = (Vertex)inp->inputparams[j+1];
//      while(deg != t && j < inp->nstats){
//       j++;
//       deg = (Vertex)inp->inputparams[j+1];
//      }
//      if(j < inp->nstats){inp->dstats[j] += echange;}
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
// *****************
// void d_gwevent
//
// *****************
void d_gwevent (int ntoggles, Vertex *heads, Vertex *tails,
		              ModelTerm *mtp, Network *nwp) {
  int i, echange=0;
  double alpha, oneexpa, change;
  Vertex h, t, hd, td=0, *id, *od;
  int nactors, nevents;

  nevents = (int)mtp->inputparams[0];
  nactors = nwp->nnodes - nevents;

  id=nwp->indegree;
  od=nwp->outdegree;
  change = 0.0;
  alpha = mtp->inputparams[1];
  oneexpa = 1.0-exp(-alpha);
//  Rprintf(" nactors %d t %d hd %d echange %d change %f\n", h, t, hd, echange,change);
  
  for (i=0; i<ntoggles; i++) 
    {
      echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
 //   hd = od[h] + id[t] + (echange - 1)/2;
      td = id[t] + (echange - 1)/2;
//      if(hd!=0){
//        change += echange*(1.0-pow(oneexpa,(double)hd));
//      }
      if(td!=0){
        change += echange*(1.0-pow(oneexpa,(double)td));
      }
//  Rprintf(" h %d t %d hd %d echange %d change %f\n", h, t, hd, echange,change);
//  Rprintf(" od[h] %d id[h] %d od[t] %d id[t] %d\n", od[h], id[h], od[t], id[t]);
      
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }

  *(mtp->dstats) = change*exp(alpha);

//  Rprintf("alpha  %f hd %d td %d change %f\n", alpha, hd, td, *(mtp->dstats));
  
  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

// *****************
// void d_gwactor
//
// *****************
void d_gwactor (int ntoggles, Vertex *heads, Vertex *tails,
		              ModelTerm *mtp, Network *nwp)
{
  int i, echange=0;
  double alpha, oneexpa, change;
  Vertex h, t, hd, td=0, *id, *od;
  int nactors, nevents;

  nactors = (int)mtp->inputparams[0];
  nevents = (nwp->nnodes) - nactors;

  id=nwp->indegree;
  od=nwp->outdegree;
  change = 0.0;
  alpha = mtp->inputparams[1];
  oneexpa = 1.0-exp(-alpha);
  
  *(mtp->dstats) = 0.0;
  for (i=0; i<ntoggles; i++) 
    {
      echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
      hd = od[h] + (echange - 1)/2;
      if(hd!=0){
        change += echange*(1.0-pow(oneexpa,(double)hd));
      }
      
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }

  *(mtp->dstats) = change*exp(alpha);

  i--; 
  while (--i>=0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

/*****************
 void d_formation
*****************/
void d_formation(int ntoggles, Vertex *heads, Vertex *tails, 
	      ModelTerm *mtp, Network *nwp) {
  int edgeflag, i;
  Vertex h, t;
  
  *(mtp->dstats) = 0.0;
  for (i=0; i < ntoggles; i++)
    {
      edgeflag = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
      *(mtp->dstats) += edgeflag ? - 1 : 1;
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }
  i--; 
  while (--i >= 0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}
/*****************
 void d_dissolve
*****************/
void d_dissolve(int ntoggles, Vertex *heads, Vertex *tails, 
	      ModelTerm *mtp, Network *nwp) {
  int edgeflag, i;
  Vertex h, t;
  
  *(mtp->dstats) = 0.0;
  for (i=0; i < ntoggles; i++)
    {
      edgeflag = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
      *(mtp->dstats) += edgeflag ? - 1 : 1;
      if (i+1 < ntoggles)
	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
    }
  i--; 
  while (--i >= 0)  /*  Undo all previous toggles. */
    ToggleEdge(heads[i], tails[i], nwp); 
}

//void d_esa (int ntoggles, Vertex *heads, Vertex *tails, 
//	      struct OptionInput *inp, Gptr g) 
//{
//  Edge e, f;
//  int i, j, echange;
//  int L2hu, L2ut;
//  Vertex deg;
//  Vertex h, t, u, v;
//  int nevents, nactors;
//
//  nactors = (int)inp->inputparams[0];
//  nevents = (nwp->nnodes) - nactors;
//
//  for (i=0; i < inp->nstats; i++) 
//    inp->dstats[i] = 0.0;
//  
//    for (i=0; i<ntoggles; i++){      
//     echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
//     //
//     // Next for actor shared event counts
//     //
////   // step through inedges of t
////   for(e = EdgetreeMinimum(nwp->inedges, t);
////       (u = nwp->inedges[e].value) != 0;
////       e = EdgetreeSuccessor(nwp->inedges, e)){
////	 if (u != h){
////          L2hu=0;
////          // step through outedges of u
////          for(f = EdgetreeMinimum(nwp->outedges, u);
////	      (v = nwp->outedges[f].value) != 0;
////           f = EdgetreeSuccessor(nwp->outedges, f)){
////               if(EdgetreeSearch(h,v,nwp->outedges)!= 0) L2hu++;
////	  }
////          for(j = 0; j < inp->nstats; j++){
////	    deg = (Vertex)inp->inputparams[j+1];
////            inp->dstats[j] += ((L2hu + echange == deg)
////			     - (L2hu == deg));
////	  }
////         }
////   }
////
////   Next for event shared actor counts
////
//   // step through outedges of h 
//   for(e = EdgetreeMinimum(nwp->outedges, h);
//       (u = nwp->outedges[e].value) != 0;
//       e = EdgetreeSuccessor(nwp->outedges, e)){
//	 if (u != t){
//          L2ut=0;
//          // step through inedges of u
//          for(f = EdgetreeMinimum(nwp->inedges, u); 
//	   (v = nwp->inedges[f].value) != 0;
//           f = EdgetreeSuccessor(nwp->inedges, f)){
//            if(EdgetreeSearch(v,t,nwp->outedges)!= 0) L2ut++;
//	  }
//          for(j = 0; j < inp->nstats; j++){
//	    deg = (Vertex)inp->inputparams[j+1];
//            inp->dstats[j] += ((L2ut + echange == deg)
//			     - (L2ut == deg));
//	  }
//         }
//   }
//     
//   if (i+1 < ntoggles)
//     ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//  }
//
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_ase (int ntoggles, Vertex *heads, Vertex *tails, 
//	      struct OptionInput *inp, Gptr g) 
//{
//  Edge e, f;
//  int i, j, echange;
//  int L2hu, L2ut;
//  Vertex deg;
//  Vertex h, t, u, v;
//  int nevents, nactors;
//
//  nactors = (int)inp->inputparams[0];
//  nevents = (nwp->nnodes) - nactors;
//
//  for (i=0; i < inp->nstats; i++) 
//    inp->dstats[i] = 0.0;
//  
//    for (i=0; i<ntoggles; i++){      
//     echange = (EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) == 0) ? 1 : -1;
//     //
//     // Next for actor shared event counts
//     //
//     // step through inedges of t
//     for(e = EdgetreeMinimum(nwp->inedges, t);
//         (u = nwp->inedges[e].value) != 0;
//         e = EdgetreeSuccessor(nwp->inedges, e)){
//  	 if (u != h){
//            L2hu=0;
//            // step through outedges of u
//            for(f = EdgetreeMinimum(nwp->outedges, u);
//  	      (v = nwp->outedges[f].value) != 0;
//             f = EdgetreeSuccessor(nwp->outedges, f)){
//                 if(EdgetreeSearch(h,v,nwp->outedges)!= 0) L2hu++;
//  	    }
//            for(j = 0; j < inp->nstats; j++){
//  	      deg = (Vertex)inp->inputparams[j+1];
//              inp->dstats[j] += ((L2hu + echange == deg)
//  			     - (L2hu == deg));
//  	    }
//         }
//    }
//     
//   if (i+1 < ntoggles)
//     ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//  }
//
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//// *****************
//// void d_bimix
////
//// *****************
//
//void d_bimix (int ntoggles, Vertex *heads, Vertex *tails, 
//	        struct OptionInput *inp, Gptr g) {
//
//  int matchvalh, matchvalt, matchswap;
//  Vertex h, t, nnodes, nstats;
//  int i, j, edgeflag=0;
//
//  nstats = inp->nstats;
//  nnodes = (inp->ninputparams)-2*nstats;
////  Rprintf("ninput %d nstats %d\n", inp->ninputparams, inp->nstats);
////  Rprintf("nodes %d\n", nnodes);
//// for (h=0; h<nnodes; h++){
////   Rprintf("match h %d att h %f\n", 
////	    h, inp->attrib[h+nstats]);
//// }
//// for (h=0; h<nstats; h++){
////   Rprintf("match h %d in h %f in t %f\n", 
////	    h, inp->inputparams[h], inp->inputparams[h+nstats]);
//// }
//  for (i=0; i < nstats; i++) 
//    inp->dstats[i] = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      h=heads[i];
//      t=tails[i];
//      matchvalh = inp->attrib[h-1+nstats];
//      matchvalt = inp->attrib[t-1+nstats];
////     if(matchvalt < matchvalh){
////       matchswap = matchvalh;
////       matchvalh = matchvalt;
////       matchvalt = matchswap;
////      }       
//      edgeflag=(EdgetreeSearch(h, t, nwp->outedges) != 0);
//      for (j=0; j<nstats; j++) 
//	  {
//           if(matchvalh==inp->inputparams[nstats+j] &&
//	      matchvalt==inp->inputparams[       j]
//	     ){inp->dstats[j] += edgeflag ? -1.0 : 1.0;}
//	  }
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_dynamic (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  Vertex h, t, hh, ht;
//  int i, k, nprevedge, edgeflag, discord, lookmore;
//  int nevents, nactors;
//  int nmat, nnodes;
//  double change=0.0;
//
//  nprevedge = (int)((inp->inputparams[0]));
//  nnodes = (nwp->nnodes);
//  nactors = (int)(inp->inputparams[1]);
//  nevents = nnodes - nactors;
//
//  nmat = nevents*nactors;
//
////  Rprintf("nprevedge %d\n", nprevedge);
////  Rprintf("nactors %d nevents %d\n", nactors, nevents);
////  for (k=1; k<=nprevedge; k++) {
////      Rprintf("k %d x0.h %d x0.t %d\n", k,
////	      (Vertex)(inp->attrib[          k]),
////	      (Vertex)(inp->attrib[nprevedge+k]));
////  }
//
//  for (i=0; i < inp->nstats; i++)
//   inp->dstats[i] = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      /*Get the initial state of the edge and its reflection*/
//      edgeflag=(EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
//      if(h > t){
//	hh = t;
//	ht = h;
//      }else{
//	hh = h;
//	ht = t;
//      }
//      discord = edgeflag ? 1 : -1;
//
//      k=1;
//      lookmore=1;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,hh,ht);
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      hh,ht,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//       if(ht == (Vertex)(inp->attrib[            k]) &&
//          hh == (Vertex)(inp->attrib[nprevedge + k])
//	 ){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//
//           /*If the proposed edge existed in x0, get dissolution rate */
//	   /* lookmore is location of (hh, ht) in the dissolve matrix */
//           lookmore=2*nprevedge+nmat+(hh-1)+(ht-1-nactors)*nevents;
//	   /* change is NEGATIVE the dissolve rate if an edge exists */
////       Rprintf("hh %d ht %d lookmore %d att %f\n",hh, ht, lookmore, (double)(inp->attrib[lookmore]));
//           change=-discord*(double)(inp->attrib[lookmore]);
////       Rprintf("lookmore %d change %f\n",lookmore, change);
//	   lookmore=0;
//           /*Update the change statistics, as appropriate*/
//	   /*change is the number of edges dissolved*/
//           inp->dstats[0] += change;
////      if(change!=0){
////       Rprintf("h %d t %d hh %d ht %d edgeflag %d beta %f\n",
////	      h,t,hh,ht, edgeflag, (double)(inp->attrib[lookmore]));
////       Rprintf("edgeflag %d change %f discord %d\n",
////	      edgeflag, change, discord);
////      }
//          }else{
//		   ++k;
//	  }
//      }
//
//      if(lookmore && nprevedge > 0){
//       /*If the proposed edge existed in x0, get dissolution rate */
//       /* lookmore, so no edge exisited in x0, Get formation rate */
//       /* lookmore is location of (hh, ht) in the form matrix */
//       lookmore=2*nprevedge+(hh-1-nactors)+(ht-1)*nevents;
//       /* change is the form number of formed edges */
//       change=-discord*(double)(inp->attrib[lookmore]);
//       /*Update the change statistics, as appropriate*/
//       inp->dstats[1] += change;
//      }
//
////      if(lookmore && change>0){
////       Rprintf("h %d t %d hh %d ht %d edgeflag %d alpha\n",
////	      h,t,hh,ht,edgeflag);
////       Rprintf("edgeflag %d change %f discord %d\n",
////	      edgeflag, change, discord);
////      }
////      /*Update the change statistics, as appropriate*/
////      *(inp->dstats) += change;
//
////  Rprintf("heads[i] %d tails[i] %d change %f \n", 
////		      heads[i], tails[i], change);
//      
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_biduration (int ntoggles, Vertex *heads, Vertex *tails, 
//	        struct OptionInput *inp, Gptr g) {
//
//  int matchvalh, matchvalt;
//  int k, nprevedge, discord, lookmore;
//  Vertex h, t, nnodes, nstats;
//  Vertex hh, ht;
//  int i, j, edgeflag=0;
//
//  nstats = inp->nstats;
////  nnodes = (inp->ninputparams)-2*nstats;
//  nnodes = (nwp->nnodes);
//  nprevedge = (int)(inp->inputparams[0]);
//  Rprintf("ninput %d nstats %d\n", inp->ninputparams, inp->nstats);
//  Rprintf("nodes %d nprevedge %d\n", nnodes, nprevedge);
//// for (h=1; h<=nnodes; h++){
////   Rprintf("match h %d att h %f\n", 
////	    h, inp->attrib[h+nstats]);
//// }
//// for (h=0; h<nstats; h++){
////   Rprintf("match h %d in h %f in t %f\n", 
////	    h, inp->inputparams[h], inp->inputparams[h+nstats]);
//// }
//  for (i=0; i < nstats; i++) 
//    inp->dstats[i] = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      h=heads[i];
//      t=tails[i];
//      matchvalh = inp->attrib[h+nstats];
//      matchvalt = inp->attrib[t+nstats];
////     if(matchvalt < matchvalh){
////       matchswap = matchvalh;
////       matchvalh = matchvalt;
////       matchvalt = matchswap;
////      }       
//      edgeflag=(EdgetreeSearch(h, t, nwp->outedges) != 0);
//      // 1 = edge removed -1 = edge added
//      discord = edgeflag ? 1 : -1;
//      k=1;
//      lookmore=1;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d h %d t %d\n",
////	        h,t,h,t);
////       Rprintf("h %d t %d h %d t %d\n",
////	      t,h,(Vertex)(inp->attrib[            k]),
////	            (Vertex)(inp->attrib[nprevedge + k]));
////     Check to see if it was a previous edge
//       if(t == (Vertex)(inp->attrib[nstats+nnodes           + k]) &&
//          h == (Vertex)(inp->attrib[nstats+nnodes+nprevedge + k])
//	 ){
////        Rprintf("h %d t %d h attr %d t attr %d\n",
////	           h,t,(Vertex)(inp->attrib[nstats+nnodes+nprevedge +   k]),
////	          (Vertex)(inp->attrib[nstats+nnodes+ k]));
////        Check to see which mixing group it is in
//          j=0;
//          while(lookmore && j < nstats){
//            if(matchvalh==inp->inputparams[nstats+j+1] &&
//	       matchvalt==inp->inputparams[      +j+1]
//	      ){
//
//              /*If the proposed edge existed in x0, get dissolution rate */
//	      /* lookmore is location of (h, t) in the dissolve matrix */
////            lookmore=2*nprevedge+(h-1)+(t-1-nactors)*nevents;
//	      /* change is NEGATIVE the dissolve rate if an edge exists */
////          Rprintf("h %d t %d lookmore %d att %f\n",h, t, lookmore, (double)(inp->attrib[lookmore]));
////              change=-discord*(double)(inp->attrib[lookmore]);
//           /*Update the change statistics, as appropriate*/
//	   /*change is the number of edges dissolved*/
//	        inp->dstats[j] -= discord;
//	        lookmore=0;
//	       }
//	       j++;
//	  }
//	  lookmore=0;
//       }
//       k++;
//      }
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_endure (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  Vertex h, t, hh, ht;
//  int i, k, nprevedge, edgeflag, discord, lookmore;
//  int nevents, nactors;
//  int nnodes;
//  double change=0.0;
//
//  nprevedge = (int)((inp->inputparams[0]));
//  nnodes = (nwp->nnodes);
//  nactors = (int)(inp->inputparams[1]);
//  nevents = nnodes - nactors;
//
////  Rprintf("nprevedge %d\n", nprevedge);
////  Rprintf("nactors %d nevents %d\n", nactors, nevents);
////  for (k=1; k<=nprevedge; k++) {
////      Rprintf("k %d x0.h %d x0.t %d\n", k,
////	      (Vertex)(inp->attrib[          k]),
////	      (Vertex)(inp->attrib[nprevedge+k]));
////  }
//
//  *(inp->dstats) = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      /*Get the initial state of the edge and its reflection*/
//      edgeflag=(EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
//      if(h > t){
//	hh = t;
//	ht = h;
//      }else{
//	hh = h;
//	ht = t;
//      }
//      // 1 = edge removed -1 = edge added
//      discord = edgeflag ? 1 : -1;
//
//// Rprintf("nprevedge %d\n", nprevedge);
//
//      k=1;
//      lookmore=1;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,hh,ht);
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      ht,hh,(Vertex)(inp->attrib[            k]),
////	            (Vertex)(inp->attrib[nprevedge + k]));
//       if(ht == (Vertex)(inp->attrib[            k]) &&
//          hh == (Vertex)(inp->attrib[nprevedge + k])
//	 ){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//
//           /*If the proposed edge existed in x0, get dissolution rate */
//	   /* lookmore is location of (hh, ht) in the dissolve matrix */
//	   /* change is NEGATIVE the dissolve rate if an edge exists */
////       Rprintf("hh %d ht %d lookmore %d att %f\n",hh, ht, lookmore, (double)(inp->attrib[lookmore]));
////           change=-discord*(double)(inp->attrib[lookmore]);
//           change=-discord;
////       Rprintf("lookmore %d change %f discord %d \n",lookmore, change, discord);
//	   lookmore=0;
//           /*Update the change statistics, as appropriate*/
//	   /*change is the number of edges dissolved*/
//           *(inp->dstats) += change;
////      if(change!=0){
////       Rprintf("h %d t %d hh %d ht %d edgeflag %d beta %f\n",
////	      h,t,hh,ht, edgeflag, (double)(inp->attrib[lookmore]));
////       Rprintf("edgeflag %d change %f discord %d\n",
////	      edgeflag, change, discord);
////      }
//          }else{
//		   ++k;
//	  }
//      }
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_bichange (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  Vertex h, t, hh, ht;
//  int i, k, nprevedge, edgeflag, discord, lookmore;
//  int nevents, nactors;
//  int nnodes;
//  double change=0.0;
//
//  nprevedge = (int)((inp->inputparams[0]));
//  nnodes = (nwp->nnodes);
//  nactors = (int)(inp->inputparams[1]);
//  nevents = nnodes - nactors;
//
////  Rprintf("nprevedge %d\n", nprevedge);
////  Rprintf("nactors %d nevents %d\n", nactors, nevents);
////  for (k=1; k<=nprevedge; k++) {
////      Rprintf("k %d x0.h %d x0.t %d\n", k,
////	      (Vertex)(inp->attrib[          k]),
////	      (Vertex)(inp->attrib[nprevedge+k]));
////  }
//
//  *(inp->dstats) = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      /*Get the initial state of the edge and its reflection*/
//      edgeflag=(EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
//      if(h > t){
//	hh = t;
//	ht = h;
//      }else{
//	hh = h;
//	ht = t;
//      }
//
//      k=1;
//      lookmore=1;
//      change=0.0;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,hh,ht);
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      hh,ht,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//       if(ht == (Vertex)(inp->attrib[            k]) &&
//          hh == (Vertex)(inp->attrib[nprevedge + k])
//	 ){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//
//           /*If the proposed edge existed in x0, get dissolution rate */
//	   /* lookmore is location of (hh, ht) in the dissolve matrix */
//           lookmore=2*nprevedge+(hh-1)+(ht-1-nactors)*nevents;
//	   /* change is NEGATIVE the dissolve rate if an edge exists */
////       Rprintf("hh %d ht %d lookmore %d att %f\n",hh, ht, lookmore, (double)(inp->attrib[lookmore]));
//           change+=edgeflag*(double)(inp->attrib[lookmore]);
////       Rprintf("lookmore %d change %f\n",lookmore, change);
//	   lookmore=0;
////      if(change!=0){
////       Rprintf("h %d t %d hh %d ht %d edgeflag %d beta %f\n",
////	      h,t,hh,ht, edgeflag, (double)(inp->attrib[lookmore]));
////       Rprintf("edgeflag %d change %f discord %d\n",
////	      edgeflag, change, discord);
////      }
//          }else{
//		   ++k;
//	  }
//
//      if(lookmore && edgeflag){
//       /* lookmore, so no edge exisited in x0, Get formation rate */
//       /* lookmore is location of (hh, ht) in the form matrix */
//       lookmore=2*nprevedge+(hh-1)+(ht-1-nactors)*nevents;
//       /* change is the form number of formed edges */
//       change+=(1-edgeflag)*(double)(inp->attrib[lookmore]);
//      }
//      /*Update the change statistics, as appropriate*/
//      /*change is the number of edges dissolved*/
//      *(inp->dstats) -= change;
//      }
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_step (int ntoggles, Vertex *heads, Vertex *tails, 
//	     struct OptionInput *inp, Gptr g) {
//
//  int matchvalh, matchvalt;
//  int k, nprevedge, discord, lookmore;
//  Vertex h, t, nnodes, nstats;
//  Vertex hh, ht;
//  Vertex ch, ct, cv, ph, pt, pv;
//  int i, j, edgeflag=0;
//
//  nstats = inp->nstats;
////  nnodes = (inp->ninputparams)-2*nstats;
//  ph1 = *(nwp->ph1);
//  pt1 = *(nwp->pt1);
//  pv1 = *(nwp->pv1);
//  ph2 = *(nwp->ph2);
//  pt2 = *(nwp->pt2);
//  pv2 = *(nwp->pv2);
//  if(pv1 != (EdgetreeSearch(ph1, pt1, nwp->outedges) != 0)
//  && pv1 != (EdgetreeSearch(ph1, pt1, nwp->outedges) != 0)){
//	  // Proposal made so update current
//   *(nwp->ch1) = ph1;
//   *(nwp->ct1) = pt1;
//   *(nwp->cv1) = pv1;
//   *(nwp->ch2) = ph2;
//   *(nwp->ct2) = pt2;
//   *(nwp->cv2) = pv2;
//  }
//  ch1 = *(nwp->ch1);
//  ct1 = *(nwp->ct1);
//  cv1 = *(nwp->cv1);
//  ch2 = *(nwp->ch2);
//  ct2 = *(nwp->ct2);
//  cv2 = *(nwp->cv2);
//
//  nnodes = (nwp->nnodes);
//  nprevedge = (int)(inp->inputparams[0]);
//  Rprintf("ninput %d nstats %d\n", inp->ninputparams, inp->nstats);
//  Rprintf("nodes %d nprevedge %d\n", nnodes, nprevedge);
//// for (h=1; h<=nnodes; h++){
////   Rprintf("match h %d att h %f\n", 
////	    h, inp->attrib[h+nstats]);
//// }
//// for (h=0; h<nstats; h++){
////   Rprintf("match h %d in h %f in t %f\n", 
////	    h, inp->inputparams[h], inp->inputparams[h+nstats]);
//// }
//  for (i=0; i < nstats; i++) 
//    inp->dstats[i] = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      h=heads[i];
//      t=tails[i];
//      edgeflag=(EdgetreeSearch(h, t, nwp->outedges) != 0);
//      // 1 = edge removed -1 = edge added
//      discord = edgeflag ? 1 : -1;
//      k=1;
//      lookmore=1;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d h %d t %d\n",
////	        h,t,h,t);
////       Rprintf("h %d t %d h %d t %d\n",
////	      t,h,(Vertex)(inp->attrib[            k]),
////	            (Vertex)(inp->attrib[nprevedge + k]));
////     Check to see if it was a previous edge
//       if(t == (Vertex)(inp->attrib[nstats+nnodes           + k]) &&
//          h == (Vertex)(inp->attrib[nstats+nnodes+nprevedge + k])
//	 ){
////        Rprintf("h %d t %d h attr %d t attr %d\n",
////	           h,t,(Vertex)(inp->attrib[nstats+nnodes+nprevedge +   k]),
////	          (Vertex)(inp->attrib[nstats+nnodes+ k]));
////        Check to see which mixing group it is in
//          j=0;
//          while(lookmore && j < nstats){
//            if(matchvalh==inp->inputparams[nstats+j+1] &&
//	       matchvalt==inp->inputparams[      +j+1]
//	      ){
//
//              /*If the proposed edge existed in x0, get dissolution rate */
//	      /* lookmore is location of (h, t) in the dissolve matrix */
////            lookmore=2*nprevedge+(h-1)+(t-1-nactors)*nevents;
//	      /* change is NEGATIVE the dissolve rate if an edge exists */
////          Rprintf("h %d t %d lookmore %d att %f\n",h, t, lookmore, (double)(inp->attrib[lookmore]));
////              change=-discord*(double)(inp->attrib[lookmore]);
//           /*Update the change statistics, as appropriate*/
//	   /*change is the number of edges dissolved*/
//	        inp->dstats[j] -= discord;
//	        lookmore=0;
//	       }
//	       j++;
//	  }
//	  lookmore=0;
//       }
//       k++;
//      }
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//
//void d_biduration_old (int ntoggles, Vertex *heads, Vertex *tails, 
//	         struct OptionInput *inp, Gptr g) 
//{
//  Vertex h, t, hh, ht;
//  int i, k, nprevedge, edgeflag, discord, lookmore;
//  int nevents, nactors;
//  int nnodes;
//  double change=0.0;
//
//  nprevedge = (int)((inp->inputparams[0]));
//  nnodes = (nwp->nnodes);
//  nactors = (int)(inp->inputparams[1]);
//  nevents = nnodes - nactors;
//
////  Rprintf("nprevedge %d\n", nprevedge);
////  Rprintf("nactors %d nevents %d\n", nactors, nevents);
////  for (k=1; k<=nprevedge; k++) {
////      Rprintf("k %d x0.h %d x0.t %d\n", k,
////	      (Vertex)(inp->attrib[          k]),
////	      (Vertex)(inp->attrib[nprevedge+k]));
////  }
//
//  *(inp->dstats) = 0.0;
//
//  for (i=0; i<ntoggles; i++) 
//    {
//      /*Get the initial state of the edge and its reflection*/
//      edgeflag=(EdgetreeSearch(h=heads[i], t=tails[i], nwp->outedges) != 0);
//      if(h > t){
//	hh = t;
//	ht = h;
//      }else{
//	hh = h;
//	ht = t;
//      }
//      // 1 = edge removed -1 = edge added
//      discord = edgeflag ? 1 : -1;
//
//// Rprintf("nprevedge %d\n", nprevedge);
//
//      k=1;
//      lookmore=1;
//      while(lookmore && k <= nprevedge){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,hh,ht);
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      ht,hh,(Vertex)(inp->attrib[            k]),
////	            (Vertex)(inp->attrib[nprevedge + k]));
//       if(ht == (Vertex)(inp->attrib[            k]) &&
//          hh == (Vertex)(inp->attrib[nprevedge + k])
//	 ){
////       Rprintf("h %d t %d hh %d ht %d\n",
////	      h,t,(Vertex)(inp->attrib[            k]),
////	          (Vertex)(inp->attrib[nprevedge + k]));
//
//           /*If the proposed edge existed in x0, get dissolution rate */
//	   /* lookmore is location of (hh, ht) in the dissolve matrix */
//           lookmore=2*nprevedge+(hh-1)+(ht-1-nactors)*nevents;
//	   /* change is NEGATIVE the dissolve rate if an edge exists */
////       Rprintf("hh %d ht %d lookmore %d att %f\n",hh, ht, lookmore, (double)(inp->attrib[lookmore]));
////           change=-discord*(double)(inp->attrib[lookmore]);
//           change=-discord;
////       Rprintf("lookmore %d change %f discord %d \n",lookmore, change, discord);
//	   lookmore=0;
//           /*Update the change statistics, as appropriate*/
//	   /*change is the number of edges dissolved*/
//           *(inp->dstats) += change;
////      if(change!=0){
////       Rprintf("h %d t %d hh %d ht %d edgeflag %d beta %f\n",
////	      h,t,hh,ht, edgeflag, (double)(inp->attrib[lookmore]));
////       Rprintf("edgeflag %d change %f discord %d\n",
////	      edgeflag, change, discord);
////      }
//          }else{
//		   ++k;
//	  }
//      }
//
//      if (i+1 < ntoggles)
//	ToggleEdge(heads[i], tails[i], nwp);  /* Toggle this edge if more to come */
//    }
//  i--; 
//  while (--i>=0)  /*  Undo all previous toggles. */
//    ToggleEdge(heads[i], tails[i], nwp); 
//}
//