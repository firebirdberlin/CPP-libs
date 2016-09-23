// Profiler.cpp
//
// Copyright 2013 Stefan Fruhner <stefan.fruhner@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.


#include "Profiler.hpp"

namespace {
// converts seconds to a string in hh:mm:ss format
void timetostr(char ttostr[], long int time){
  double std, min, s;
  char zeichen[255];
//  TELL(time);
  std = ((time)/3600.);
  min = fmod((time/60.),60.);
	s   = fmod(time ,60.0);
        if (std < 10) sprintf(zeichen," 0%0.0f:",floor(std)); else sprintf(zeichen," %0.0f:",floor(std));
	strcpy(ttostr,zeichen);
	if (min < 10) sprintf(zeichen,"0%0.0f:",floor(min) ); else sprintf(zeichen,"%0.0f:",floor(min));
	strcat(ttostr,zeichen);
	if (s < 10) sprintf(zeichen,"0%0.0f ", floor(s) ); else sprintf(zeichen,"%0.0f ", floor(s) );
	strcat(ttostr,zeichen);
}

void time_info(int *progress, const int total, int reference_time, int *itime, const int errors = 0) {

#ifdef _OPENMP
		#pragma omp critical
		{
			(*progress)++;

			if (omp_get_thread_num() == 0)
			{
				double diff    = omp_get_wtime() - *itime;
				double elapsed = omp_get_wtime() - reference_time;
				double pc =(double) (*progress)/ (double) total * 100.;
				if (diff > 1. or (*progress == total) )	{ // print info every second
//				if (diff > 10. ) { // print info every ten seconds
					*itime = omp_get_wtime();
//					printf("\r%7.2f %%  (error rate:%7.2f %%, %2d threads ), time:%7.2f s /%7.2f s ",
//						pc,
//						(double) errors/ *progress * 100.,
//						omp_get_num_threads(),
//						elapsed,
//						elapsed  / pc * (100-pc));
					printf("\r%7.2f %%",pc);
					if (errors > 0)
						printf(" (error rate:%7.2f %%, ",	(double) errors/ *progress * 100.);
					else
						printf(" (");
					double remaining = elapsed  / pc * (100.-pc);
//					if (remaining < 60.){
//						printf("%2d threads ), time:%7.2f s /%7.2f s ",
//							omp_get_num_threads(),
//							elapsed,
//							elapsed  / pc * (100.-pc));
//					} else {
						char tm1[256];
						char tm2[256];
						timetostr(tm1, (long int) elapsed);
						timetostr(tm2, (long int) remaining);
//						printf("%s", tm1);
						printf("%2d threads ), time: %s/%s ",
									omp_get_num_threads(),
									tm1, tm2);
//					}
					fflush(stdout);
		       }

		       if (*progress == total) printf("\n");
			}
		}
#else
		*progress++;
		if (*progress % 100 == 0 or (*progress == total)) { // every 100 steps
			printf("\r\t%3.2lf %%  (error rate : %3.2lf %%)  ",
				(double) *progress/total * 100.,
				(double) errors/ i * 100);
		}
#endif

}

int measure_time_reference(int *stime){
#ifdef _OPENMP
	*stime = omp_get_wtime();
	return *stime;
#endif
}
} // end of anonymous namespace

// must be created before the start of a loop
Profiler::Profiler(size_t total_items){
	reset(total_items);
}

void Profiler::finalize(){
//	printf("\n%d / %d \n", progress, total);
//	std::cout << progress << " / " << total << std::endl;

	if (total != progress+1) {
		printf("\n");
		progress = total-1;
	}
}

void Profiler::reset(size_t total_items){
	if (total != progress+1) printf("\n");
	total 	 = total_items;
	progress = 0;
	tstart	 = 0;
	tend 	 = measure_time_reference(&tstart);
}

// must be called once in a loop
void Profiler::info(){
	time_info(&progress, total, tstart, &tend);
}
