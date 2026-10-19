# C allocation tracker, utility
This library provides you with functions like `gc_malloc()` and `gc_realloc()` that keep track of the stdlib function returned pointers and frees them all on `gc_free_all()`.
