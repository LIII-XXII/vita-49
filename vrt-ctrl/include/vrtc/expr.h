/* -*- c -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_VRTC_EXPR_H
#define INCLUDED_VRTC_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <vrtc/asn1c/Expr.h>
#include <vrtc/datagram_buffer.h>

  /* ----- Constructors ----- */

/*! \returns a "null" expr if successful, else 0. */
Expr_t *expr_make_null(void);

/*! \returns a "bool" expr if successful, else 0. */
Expr_t * expr_make_bool(bool v);

/*! \returns a "integer" expr if successful, else 0. */
Expr_t * expr_make_int(long v);

/*! 
 * \returns a "string" expr if successful, else 0.
 * \param[in] p pointer to bytes to copy into expr
 * \param[in] len number of bytes to copy
 */
Expr_t *expr_make_string(const char *p, size_t len);

/*!
 * \Returns a "string" expr if successful, else 0.
 * \param[in] p pointer to zero-terminated string
 */
Expr_t * expr_make_cstring(const char *p);

/*! \Returns a sequence with 0 elements if successful, else 0. */
Expr_t *expr_make_seq0(void);

/*! 
 * \brief Returns a sequence with 1 element \p x0 if successful, else 0.
 * If any args are zero, free all non-zero args and return 0.
 */
Expr_t *expr_make_seq1(Expr_t *x0);

/*! 
 * \brief Returns a sequence with 2 elements \p x0, \p x1 if successful, else 0.
 * If any args are zero, free all non-zero args and return 0.
 */
Expr_t *expr_make_seq2(Expr_t *x0, Expr_t *x1);

/*! 
 * \brief Returns a sequence with 3 elements \p x0, \p x1, \p x2 if successful, else 0.
 * If any args are zero, free all non-zero args and return 0.
 */
Expr_t *expr_make_seq3(Expr_t *x0, Expr_t *x1, Expr_t *x2);

/*! 
 * \brief Returns a sequence with 4 elements \p x0, \p x1,\p x2, \p x3 if successful, else 0.
 * If any args are zero, free all non-zero args and return 0.
 */
Expr_t *expr_make_seq4(Expr_t *x0, Expr_t *x1, Expr_t *x2, Expr_t *x3);

/*!
 * \brief Returns a zero length sequence to which elements can be added
 * using vrtc_seq_add_element.
 */
Expr_t *expr_make_seq(void);

/*!
 * \brief Add element to seq.
 *
 * \Returns seq if successful, else frees element and seq and returns 0.
 */
Expr_t *expr_seq_add_element(Expr_t *seq, Expr_t *element);

  /* ----- Predicates ----- */

  static inline bool expr_is_seq(Expr_t *x) { return x->present == Expr_PR_seq; }
  static inline bool expr_is_null(Expr_t *x) { return x->present == Expr_PR_null; }
  static inline bool expr_is_bool(Expr_t *x) { return x->present == Expr_PR_boolean; }
  static inline bool expr_is_int(Expr_t *x) { return x->present == Expr_PR_integer; }
  static inline bool expr_is_string(Expr_t *x) { return x->present == Expr_PR_string; }
  static inline bool expr_is_float(Expr_t *x) { return x->present == Expr_PR_ieee_float; }  
  static inline bool expr_is_complex_int(Expr_t *x) { return x->present == Expr_PR_complex_int; }
  static inline bool expr_is_complex_float(Expr_t *x) { return x->present == Expr_PR_complex_float; }

  /* ----- Accessors ----- */

  /* Caller MUST confirm type using predicate before applying accessor */

  static inline unsigned int
  expr_seq_len(Expr_t *x)
  {
    return x->choice.seq.list.count;
  }

  static inline Expr_t *
  expr_seq_ref(Expr_t *x, size_t index)
  {
    return x->choice.seq.list.array[index];
  }

  static inline bool
  expr_get_bool(Expr_t *x)
  {
    return x->choice.boolean;
  }

  static inline long
  expr_get_int(Expr_t *x)
  {
    return x->choice.integer;
  }

  static inline unsigned int
  expr_string_len(Expr_t *x)
  {
    return x->choice.string.size;
  }

  static inline unsigned char *
  expr_string_ptr(Expr_t *x)
  {
    return x->choice.string.buf;
  }

  /* ----- Misc ----- */

/*! 
 * \brief Free Expr_t \p p and any contained elements 
 * If \p is 0, nothing is done.
 */
void expr_free(Expr_t *p);


/*!
 * \brief Return a copy of x iff successful, else 0.
 */
Expr_t *expr_clone(Expr_t *x);

/* print a representation of \x to stdout */
void expr_print(Expr_t *x);

/*
 * -------------------------------------------------------------------------
 * Routines to encode and decode between Expr_t's and the on-the-wire format
 * -------------------------------------------------------------------------
 */

typedef asn_dec_rval_t  expr_dec_rval_t;

/*!
 * \brief encode an Expr_t into the on-the-wire format
 *
 * \param[input] e is the Expr_t to be encoded
 * \param[input] dest is the datagram_buffer where the encoded Expr_t is written.
 *
 * \returns true iff \p e was encoded successfully.
 *
 * It will fail if there's a problem with \p e, or if the encoded representation
 * is bigger than the maximum datagram.
 */
bool
expr_encode(Expr_t *e, datagram_buffer_t *dest);

/*!
 * \brief encode an Expr_t into the on-the-wire format and free it.
 *
 * \param[input] e is the Expr_t to be encoded, then freed
 * \param[input] dest is the datagram_buffer where the encoded Expr_t is written.
 *
 * \returns true iff \p e was encoded successfully.
 *
 * It will fail if there's a problem with \p e, or if the encoded representation
 * is bigger than the maximum datagram.
 */
bool
expr_encode_and_free(Expr_t *e, datagram_buffer_t *dest);
  
/*!
 * \brief decode on-the-wire format into an Expr_t
 *
 * \param[output] e is the decoded Expr_t
 * \param[input] buffer is the data to be decoded
 * \param[input] size is the number of bytes in \p buffer
 */
expr_dec_rval_t
expr_decode(Expr_t **e,
	    const void *buffer,
	    size_t size);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDED_VRTC_EXPR_H */
