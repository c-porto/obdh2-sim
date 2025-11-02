#include <pthread.h>

#include <predict/predict.h>
#include <predict/unsorted.h>

#include <system/context.h>
#include <system/db.h>
#include <system/sys_log.h>

void *pos_det_thread(void *arg)
{
	struct obdh_sim_ctx *ctx = arg;
	struct timespec next = { 0 };
	struct db_handle db = { 0 };
	const char *db_file = "/var/local/pos_det.sqlite3";

	if (create_tm_db(&db, db_file) < 0)
		sys_log_print_event_from_module(SYS_LOG_ERROR, "pos_det",
						"Failed to create DB!");

	predict_orbital_elements_t satellite;
	struct predict_sgp4 sgp4_model;
	struct predict_sdp4 sdp4_model;

	/* Pointer used to see if TLE parsing was sucessfull */
	predict_orbital_elements_t *sat = NULL;

	/* HORYU-4 TLE line */
	const char *line1 =
		"1 41340U 16012D   25295.28377617  .00049187  00000+0  10033-2 0  9993";
	const char *line2 =
		"2 41340  30.9957 301.7129 0003064  49.3366 310.7548 15.44938490533572";

	sat = predict_parse_tle(&satellite, &sgp4_model, &sdp4_model, line1,
				line2);

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 60;

		if (sat != NULL) {
			/* Predict satellite position */
			struct predict_position my_orbit;

			predict_julian_date_t curr_time =
				julian_from_timestamp(time(NULL));

			(void)predict_orbit(&satellite, &my_orbit, curr_time);

			float lat = predictRAD2DEG(my_orbit.latitude);
			float lon = predictRAD2DEG(my_orbit.longitude);
			float alt = my_orbit.altitude;

			sys_log_print_event_from_module(
				SYS_LOG_INFO, "pos",
				"Current position (lat/lon/alt): %.3f deg/%.3f deg/%.3f km",
				lat, lon, alt);

			/* Context is shared between threads */
			pthread_mutex_lock(&ctx->lock);
			ctx->cond.eclipsed = my_orbit.eclipsed;
			pthread_mutex_unlock(&ctx->lock);

			if (db.handle) {
				tm_db_add_entry(&db, "pos_det", "lat", lat);
				tm_db_add_entry(&db, "pos_det", "lon", lon);
				tm_db_add_entry(&db, "pos_det", "alt", alt);
				tm_db_add_entry(&db, "pos_det", "eclipsed", (double)my_orbit.eclipsed);
			}
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "pos",
				"Failed to parse last available TLEs!");
		}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	};

	return NULL;
}
