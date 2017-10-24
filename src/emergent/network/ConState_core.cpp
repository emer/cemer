// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in _cpp.cpp, _cuda.cpp

int CON_STATE::ConnectUnOwnCn(NETWORK_STATE* net, UNIT_STATE* un, bool ignore_alloc_errs,
                              bool allow_null_unit) {
  static bool warned_already = false;
  if(!allow_null_unit && un->flat_idx == 0) { return -1; } // null unit -- don't even connect!
  if(size >= alloc_size) {
    if(!ignore_alloc_errs && !warned_already) {
      PRJN_STATE* prjn = GetPrjnState(net);
      net->StateErrorVals("ConnectUnOwnCn: mem not allocated or size already at maximum allocated -- this is a programmer error -- please report", "alloc_size", alloc_size, "recv_lay_idx", prjn->recv_lay_idx,
                          "send_lay_idx", prjn->send_lay_idx);
    }
    warned_already = true;   return -1;
  }
  warned_already = false;
  int rval = size;
  UnIdx(size++) = (int)un->flat_idx;
  return rval;
}

bool CON_STATE::ConnectUnPtrCn(NETWORK_STATE* net, UNIT_STATE* un, int con_idx, bool ignore_alloc_errs) {
  static bool warned_already = false;
  if(un->flat_idx == 0) { return false; } // null unit -- don't even connect!
  if(size >= alloc_size) {
    if(!ignore_alloc_errs && !warned_already) {
      PRJN_STATE* prjn = GetPrjnState(net);
      net->StateErrorVals("ConnectUnPtrCn: mem not allocated or size already at maximum allocated -- this is a programmer error -- please report", "alloc_size", alloc_size, "recv_lay_idx", prjn->recv_lay_idx,
                          "send_lay_idx", prjn->send_lay_idx);
    }
    warned_already = true;    return false;
  }
  warned_already = false;
  PtrCnIdx(size) = con_idx;
  UnIdx(size++) = (int)un->flat_idx;
  return true;
}

int CON_STATE::ConnectUnits
(NETWORK_STATE* net, UNIT_STATE* our_un, UNIT_STATE* oth_un, CON_STATE* oth_cons,
 bool ignore_alloc_errs,  bool set_init_wt, float init_wt) {
  int con = -1;
  if(OwnCons()) {
    con = ConnectUnOwnCn(net, oth_un, ignore_alloc_errs);
    if(con >= 0) {
      if(!oth_cons->ConnectUnPtrCn(net, our_un, con, ignore_alloc_errs)) {
        con = -1; RemoveConIdx(size-1, net);   // remove last guy!  otherwise it is a dangler
      }
    }
    if(con >= 0 && set_init_wt) {
      SafeFastCn(con, WT, net) = init_wt;
    }
  }
  else {
    con = oth_cons->ConnectUnOwnCn(net, our_un, ignore_alloc_errs);
    if(con >= 0) {
      if(!ConnectUnPtrCn(net, oth_un, con, ignore_alloc_errs)) {
        con = -1;
        oth_cons->RemoveConIdx(size-1, net); // remove last guy!  otherwise it is a dangler
      }
    }
    if(con >= 0 && set_init_wt) {
      SafeFastCn(size-1, WT, net) = init_wt; // our connection is last one: size-1
    }
  }
  return con;
}

bool CON_STATE::SetShareFrom(NETWORK_STATE* net, UNIT_STATE* shu) {
  if(shu->flat_idx >= own_flat_idx) {
    net->StateError("SetShareFrom: share source unit must be earlier in network than sharing unit");
    return false;
  }
  if(net->UnThr(shu->flat_idx) != net->UnThr(own_flat_idx)) {
    net->StateError("SetShareFrom: share source and this unit must be on same thread -- requires unit groups to be even multiple of number of threads!");
    return false;
  }
  share_idx = shu->flat_idx;
  return true;
}

void CON_STATE::VecChunk_SendOwns(NETWORK_STATE* net, 
                                 int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem) {
  vec_chunked_size = 0;
  if(!IsSend() || !OwnCons()) {  // must be these two things for this to work
    return;
  }
  if(vec_chunk_targ <= 1 || size < vec_chunk_targ || mem_start == 0) {
    return;
  }

  int first_change = VecChunk_impl(tmp_chunks, tmp_not_chunks, tmp_con_mem);

  // todo!
  // fix all the other guy con pointers
  // if(first_change >= 0 && first_change < size) {
  //   FixConPtrs_SendOwns(net, first_change);
  // }
}

void CON_STATE::VecChunk_RecvOwns(NETWORK_STATE* net, 
                                 int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem) {
  vec_chunked_size = 0;
  if(!IsRecv() || !OwnCons()) {  // must be these two things for this to work
    return;
  }
  if(vec_chunk_targ <= 1 || size < vec_chunk_targ || mem_start == 0) {
    return;
  }

  int first_change = VecChunk_impl(tmp_chunks, tmp_not_chunks, tmp_con_mem);

  // todo!
  // fix all the other guy con pointers
  // if(first_change >= 0 && first_change < size) {
  //   FixConPtrs_RecvOwns(net, first_change);
  // }
}


int CON_STATE::VecChunk_impl(int* tmp_chunks, int* tmp_not_chunks,
                            float* tmp_con_mem) {
  vec_chunked_size = 0;

  // first find all the chunks and not-chunks
  int n_chunks = 0;
  int n_not_chunks = 0;
  int prv_uni = -1;
  int cur_seq_st = -1;
  int i;
  for(i=0; i < size; i++) {
    int uni = UnIdx(i);
    if(prv_uni < 0) {
      prv_uni = uni;
      continue;                 // new start
    }
    if(uni == prv_uni+1) {      // sequential
      prv_uni = uni;
      if(cur_seq_st < 0) {
        cur_seq_st = i-1;       // new chunk start..
      }
      else {
        if(i-cur_seq_st == (vec_chunk_targ-1)) { // got a chunk's worth
          tmp_chunks[n_chunks] = cur_seq_st; // record
          n_chunks++;
          prv_uni = -1;
          cur_seq_st = -1;      // start again
        }
        // otherwise keep going..
      }
    }
    else {
      prv_uni = uni;
      if(cur_seq_st < 0) {
        // last guy is definitely a non-chunk
        tmp_not_chunks[n_not_chunks++] = i-1;
      }
      else {
        // everybody from cur_seq_st to before me is a non-matcher
        for(int j=cur_seq_st; j < i; j++) {
          tmp_not_chunks[n_not_chunks++] = j;
        }
        cur_seq_st = -1;          // reset
      }
    }
  }
  if(prv_uni >= 0) {            // was working on something
    if(cur_seq_st < 0) {
      // last guy is definitely a non-chunk
      tmp_not_chunks[n_not_chunks++] = i-1;
    }
    else {
      // everybody from cur_seq_st to before me is a non-matcher
      for(int j=cur_seq_st; j < i; j++) {
        tmp_not_chunks[n_not_chunks++] = j;
      }
    }
  }

  if(n_chunks == 0)
    return size;                // no changes
  
  int ncv = NConVars()+1;       // include unit idx
  if(Sharing())
    ncv = 1;                    // other guy must have done it -- we just do our ptrs

  // now construct new reorganized data
  int cur_sz = 0;
  for(i=0; i<n_chunks; i++) {
    int seq_st = tmp_chunks[i];
    for(int j=seq_st; j< seq_st+vec_chunk_targ; j++) {
      for(int v=0; v<ncv; v++) {
        tmp_con_mem[alloc_size * v + cur_sz] = mem_start[alloc_size * v + j];
      }
      cur_sz++;
    }
  }
  for(i=0; i<n_not_chunks; i++) {
    int j = tmp_not_chunks[i];
    for(int v=0; v<ncv; v++) {
      tmp_con_mem[alloc_size * v + cur_sz] = mem_start[alloc_size * v + j];
    }
    cur_sz++;
  }

#ifdef DEBUG
  if(cur_sz != size) {
    taMisc::Error("VecChunk_SendOwns: new size != orig size -- oops!");
    return size;
  }
#endif

  int first_change = -1;
  for(i=0; i<size; i++) {
    if(UnIdx(i) != ((int*)tmp_con_mem)[i]) {
      first_change = i;
      break;
    }
  }

  // then copy over the newly reorganized guys..
  for(int j=0; j< ncv; j++) {
    memcpy(MemBlock(j), (char*)(tmp_con_mem + alloc_size * j), size * sizeof(float));
  }

  vec_chunked_size = n_chunks * vec_chunk_targ; // we are now certfied chunkable..

  return first_change;
}

float& CON_STATE::SafeCn(NETWORK_STATE* net, int idx, int var_no) const {
  if(!InRange(idx)) {
    net->StateErrorVals("SafeCn: index out of range:", "idx:", idx, "size:", size);
    return const_cast<float&>(temp1);
  }
  if(!VarInRange(var_no)) {
    net->StateErrorVals("SafeCn: variable number out of range:", "var_no:", var_no,
                       "number of variables:", n_con_vars);
    return const_cast<float&>(temp1);
  }
  if(OwnCons()) {
    return OwnCn(idx, var_no);
  }
  if(NotActive()) return const_cast<float&>(temp1);
  return UnCons(idx, net)->SafeCn(net, PtrCnIdx(idx), var_no);
}

float& CON_STATE::SafeCnName(NETWORK_STATE* net, int idx, const char* var_nm) const {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, var_nm);
  if(var_no < 0) {
    net->StateError("SafeCnName: variable named:", var_nm,
                    "not found in connection");
    return const_cast<float&>(temp1);
  }
  return SafeCn(net, idx, var_no);
}

bool CON_STATE::SetCnVal(NETWORK_STATE* net, float val, int idx, int var_no) {
  if(!InRange(idx)) {
    net->StateErrorVals("SetCnVal: index out of range:", "idx:", idx,
                        "size:", size);
    return false;
  }
  if(!VarInRange(var_no)) {
    net->StateErrorVals("SafeCnVal: variable number out of range:", "var_no:", var_no,
                       "number of variables:", n_con_vars);
    return false;
  }
  if(NotActive()) return false;
  Cn(idx, var_no, net) = val;
  return true;
}

bool CON_STATE::SetCnValName(NETWORK_STATE* net, float val, int idx, const char* var_nm) {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, var_nm);
  if(var_no < 0) {
    net->StateError("SafeCnName: variable named:", var_nm,
                    "not found in connection");
    return false;
  }
  return SetCnVal(net, val, idx, var_no);
}

