/* -*- c-basic-offset: 2; indent-tabs-mode: nil -*- */
#ifndef DVDREAD_INTERNAL_H
#define DVDREAD_INTERNAL_H


#define CHECK_VALUE(arg)                                                \
  if(!(arg)) {                                                          \
    fprintf(stderr, "\n*** libdvdread: CHECK_VALUE failed in %s:%i ***" \
            "\n*** for %s ***\n\n",                                     \
            __FILE__, __LINE__, # arg );                                \
  }


int ___get_verbose(void);
#define get_verbose() ___get_verbose()
int ___dvdread_verbose(dvd_reader_t *dvd);
#define dvdread_verbose(a) ___dvdread_verbose(a)
dvd_reader_t *___device_of_file(dvd_file_t *file);
#define device_of_file(a) ___device_of_file(a)
#endif /* DVDREAD_INTERNAL_H */
