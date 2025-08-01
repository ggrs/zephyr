/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 Carl Zeiss Meditec AG
 * SPDX-FileCopyrightText: Copyright (c) 2024 Jilay Sandeep Pandya
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file drivers/stepper.h
 * @brief Public API for Stepper Driver
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_STEPPER_H_
#define ZEPHYR_INCLUDE_DRIVERS_STEPPER_H_

/**
 * @brief Stepper Driver Interface
 * @defgroup stepper_interface Stepper Driver Interface
 * @since 4.0
 * @version 0.1.0
 * @ingroup io_interfaces
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stepper Motor micro-step resolution options
 */
enum stepper_micro_step_resolution {
	/** Full step resolution */
	STEPPER_MICRO_STEP_1 = 1,
	/** 2 micro-steps per full step */
	STEPPER_MICRO_STEP_2 = 2,
	/** 4 micro-steps per full step */
	STEPPER_MICRO_STEP_4 = 4,
	/** 8 micro-steps per full step */
	STEPPER_MICRO_STEP_8 = 8,
	/** 16 micro-steps per full step */
	STEPPER_MICRO_STEP_16 = 16,
	/** 32 micro-steps per full step */
	STEPPER_MICRO_STEP_32 = 32,
	/** 64 micro-steps per full step */
	STEPPER_MICRO_STEP_64 = 64,
	/** 128 micro-steps per full step */
	STEPPER_MICRO_STEP_128 = 128,
	/** 256 micro-steps per full step */
	STEPPER_MICRO_STEP_256 = 256,
};

/**
 * @brief Macro to calculate the index of the microstep resolution
 * @param res Microstep resolution
 */
#define MICRO_STEP_RES_INDEX(res) LOG2(res)

#define VALID_MICRO_STEP_RES(res)                                                                  \
	((res) == STEPPER_MICRO_STEP_1 || (res) == STEPPER_MICRO_STEP_2 ||                         \
	 (res) == STEPPER_MICRO_STEP_4 || (res) == STEPPER_MICRO_STEP_8 ||                         \
	 (res) == STEPPER_MICRO_STEP_16 || (res) == STEPPER_MICRO_STEP_32 ||                       \
	 (res) == STEPPER_MICRO_STEP_64 || (res) == STEPPER_MICRO_STEP_128 ||                      \
	 (res) == STEPPER_MICRO_STEP_256)

/**
 * @brief Stepper Motor direction options
 */
enum stepper_direction {
	/** Negative direction */
	STEPPER_DIRECTION_NEGATIVE = 0,
	/** Positive direction */
	STEPPER_DIRECTION_POSITIVE = 1,
};

/**
 * @brief Stepper Motor run mode options
 */
enum stepper_run_mode {
	/** Hold Mode */
	STEPPER_RUN_MODE_HOLD = 0,
	/** Position Mode*/
	STEPPER_RUN_MODE_POSITION = 1,
	/** Velocity Mode */
	STEPPER_RUN_MODE_VELOCITY = 2,
};

/**
 * @brief Stepper Events
 */
enum stepper_event {
	/** Steps set using move_by or move_to have been executed */
	STEPPER_EVENT_STEPS_COMPLETED = 0,
	/** Stall detected */
	STEPPER_EVENT_STALL_DETECTED = 1,
	/** Left end switch status changes to pressed */
	STEPPER_EVENT_LEFT_END_STOP_DETECTED = 2,
	/** Right end switch status changes to pressed */
	STEPPER_EVENT_RIGHT_END_STOP_DETECTED = 3,
	/** Stepper has stopped */
	STEPPER_EVENT_STOPPED = 4,
	/** Fault with the stepper controller detected */
	STEPPER_EVENT_FAULT_DETECTED = 5,
};

/**
 * @cond INTERNAL_HIDDEN
 *
 * Stepper driver API definition and system call entry points.
 *
 */

/**
 * @brief Enable the stepper driver.
 *
 * @see stepper_enable() for details.
 */
typedef int (*stepper_enable_t)(const struct device *dev);

/**
 * @brief Disable the stepper driver.
 *
 * @see stepper_disable() for details.
 */
typedef int (*stepper_disable_t)(const struct device *dev);

/**
 * @brief Set the micro-step resolution
 *
 * @see stepper_set_micro_step_res() for details.
 */
typedef int (*stepper_set_micro_step_res_t)(const struct device *dev,
					    const enum stepper_micro_step_resolution resolution);

/**
 * @brief Get the micro-step resolution
 *
 * @see stepper_get_micro_step_res() for details.
 */
typedef int (*stepper_get_micro_step_res_t)(const struct device *dev,
					    enum stepper_micro_step_resolution *resolution);
/**
 * @brief Set the reference position of the stepper
 *
 * @see stepper_set_actual_position() for details.
 */
typedef int (*stepper_set_reference_position_t)(const struct device *dev, const int32_t value);

/**
 * @brief Get the actual a.k.a reference position of the stepper
 *
 * @see stepper_get_actual_position() for details.
 */
typedef int (*stepper_get_actual_position_t)(const struct device *dev, int32_t *value);

/**
 * @brief Callback function for stepper events
 */
typedef void (*stepper_event_callback_t)(const struct device *dev, const enum stepper_event event,
					 void *user_data);

/**
 * @brief Set the callback function to be called when a stepper event occurs
 *
 * @see stepper_set_event_callback() for details.
 */
typedef int (*stepper_set_event_callback_t)(const struct device *dev,
					    stepper_event_callback_t callback, void *user_data);
/**
 * @brief Set the time interval between steps in nanoseconds.
 *
 * @see stepper_set_microstep_interval() for details.
 */
typedef int (*stepper_set_microstep_interval_t)(const struct device *dev,
						const uint64_t microstep_interval_ns);
/**
 * @brief Move the stepper relatively by a given number of micro-steps.
 *
 * @see stepper_move_by() for details.
 */
typedef int (*stepper_move_by_t)(const struct device *dev, const int32_t micro_steps);

/**
 * @brief Move the stepper to an absolute position in micro-steps.
 *
 * @see stepper_move_to() for details.
 */
typedef int (*stepper_move_to_t)(const struct device *dev, const int32_t micro_steps);

/**
 * @brief Run the stepper with a given step interval in a given direction
 *
 * @see stepper_run() for details.
 */
typedef int (*stepper_run_t)(const struct device *dev, const enum stepper_direction direction);

/**
 * @brief Stop the stepper
 *
 * @see stepper_stop() for details.
 */
typedef int (*stepper_stop_t)(const struct device *dev);

/**
 * @brief Is the target position fo the stepper reached
 *
 * @see stepper_is_moving() for details.
 */
typedef int (*stepper_is_moving_t)(const struct device *dev, bool *is_moving);

/**
 * @brief Stepper Driver API
 */
__subsystem struct stepper_driver_api {
	stepper_enable_t enable;
	stepper_disable_t disable;
	stepper_set_micro_step_res_t set_micro_step_res;
	stepper_get_micro_step_res_t get_micro_step_res;
	stepper_set_reference_position_t set_reference_position;
	stepper_get_actual_position_t get_actual_position;
	stepper_set_event_callback_t set_event_callback;
	stepper_set_microstep_interval_t set_microstep_interval;
	stepper_move_by_t move_by;
	stepper_move_to_t move_to;
	stepper_run_t run;
	stepper_stop_t stop;
	stepper_is_moving_t is_moving;
};

/**
 * @endcond
 */

/**
 * @brief Enable stepper driver
 *
 * @details Enabling the driver shall switch on the power stage and energize the coils.
 *
 * @param dev pointer to the stepper driver instance
 *
 * @retval -EIO Error during Enabling
 * @retval 0 Success
 */
__syscall int stepper_enable(const struct device *dev);

static inline int z_impl_stepper_enable(const struct device *dev)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	return api->enable(dev);
}

/**
 * @brief Disable stepper driver
 *
 * @details Disabling the driver shall switch off the power stage and de-energize the coils.
 * Disabling the stepper does not implicitly stop the stepper. If the motor shall not move after
 * re-enabling the stepper than consider calling stepper_stop() before.
 *
 * @param dev pointer to the stepper driver instance
 *
 * @retval  -ENOTSUP Disabling of driver is not supported.
 * @retval -EIO Error during Disabling
 * @retval 0 Success
 */
__syscall int stepper_disable(const struct device *dev);

static inline int z_impl_stepper_disable(const struct device *dev)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	return api->disable(dev);
}

/**
 * @brief Set the micro-step resolution in stepper driver
 *
 * @param dev pointer to the stepper driver instance
 * @param resolution micro-step resolution
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval -EINVAL If the requested resolution is invalid
 * @retval -ENOTSUP If the requested resolution is not supported
 * @retval 0 Success
 */
__syscall int stepper_set_micro_step_res(const struct device *dev,
					 enum stepper_micro_step_resolution resolution);

static inline int z_impl_stepper_set_micro_step_res(const struct device *dev,
						    enum stepper_micro_step_resolution resolution)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->set_micro_step_res == NULL) {
		return -ENOSYS;
	}

	if (!VALID_MICRO_STEP_RES(resolution)) {
		return -EINVAL;
	}
	return api->set_micro_step_res(dev, resolution);
}

/**
 * @brief Get the micro-step resolution in stepper driver
 *
 * @param dev pointer to the stepper driver instance
 * @param resolution micro-step resolution
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_get_micro_step_res(const struct device *dev,
					 enum stepper_micro_step_resolution *resolution);

static inline int z_impl_stepper_get_micro_step_res(const struct device *dev,
						    enum stepper_micro_step_resolution *resolution)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->get_micro_step_res == NULL) {
		return -ENOSYS;
	}
	return api->get_micro_step_res(dev, resolution);
}

/**
 * @brief Set the reference position of the stepper
 *
 * @param dev Pointer to the stepper driver instance.
 * @param value The reference position to set in micro-steps.
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_set_reference_position(const struct device *dev, int32_t value);

static inline int z_impl_stepper_set_reference_position(const struct device *dev,
							const int32_t value)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->set_reference_position == NULL) {
		return -ENOSYS;
	}
	return api->set_reference_position(dev, value);
}

/**
 * @brief Get the actual a.k.a reference position of the stepper
 *
 * @param dev pointer to the stepper driver instance
 * @param value The actual position to get in micro-steps
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_get_actual_position(const struct device *dev, int32_t *value);

static inline int z_impl_stepper_get_actual_position(const struct device *dev, int32_t *value)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->get_actual_position == NULL) {
		return -ENOSYS;
	}
	return api->get_actual_position(dev, value);
}

/**
 * @brief Set the callback function to be called when a stepper event occurs
 *
 * @param dev pointer to the stepper driver instance
 * @param callback Callback function to be called when a stepper event occurs
 * passing NULL will disable the callback
 * @param user_data User data to be passed to the callback function
 *
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_set_event_callback(const struct device *dev,
					 stepper_event_callback_t callback, void *user_data);

static inline int z_impl_stepper_set_event_callback(const struct device *dev,
						    stepper_event_callback_t callback,
						    void *user_data)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->set_event_callback == NULL) {
		return -ENOSYS;
	}
	return api->set_event_callback(dev, callback, user_data);
}

/**
 * @brief Set the time interval between steps in nanoseconds with immediate effect.
 *
 * @note Setting step interval does not set the stepper into motion, a combination of
 * set_microstep_interval and move is required to set the stepper into motion.
 *
 * @param dev pointer to the stepper driver instance
 * @param microstep_interval_ns time interval between steps in nanoseconds
 *
 * @retval -EIO General input / output error
 * @retval -EINVAL If the requested step interval is not supported
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_set_microstep_interval(const struct device *dev,
					     uint64_t microstep_interval_ns);

static inline int z_impl_stepper_set_microstep_interval(const struct device *dev,
							const uint64_t microstep_interval_ns)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->set_microstep_interval == NULL) {
		return -ENOSYS;
	}
	return api->set_microstep_interval(dev, microstep_interval_ns);
}

/**
 * @brief Set the micro-steps to be moved from the current position i.e. relative movement
 *
 * @details The stepper will move by the given number of micro-steps from the current position.
 * This function is non-blocking.
 *
 * @param dev pointer to the stepper driver instance
 * @param micro_steps target micro-steps to be moved from the current position
 *
 * @retval -EIO General input / output error
 * @retval 0 Success
 */
__syscall int stepper_move_by(const struct device *dev, int32_t micro_steps);

static inline int z_impl_stepper_move_by(const struct device *dev, const int32_t micro_steps)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	return api->move_by(dev, micro_steps);
}

/**
 * @brief Set the absolute target position of the stepper
 *
 * @details The stepper will move to the given micro-steps position from the reference position.
 * This function is non-blocking.
 *
 * @param dev pointer to the stepper driver instance
 * @param micro_steps target position to set in micro-steps
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_move_to(const struct device *dev, int32_t micro_steps);

static inline int z_impl_stepper_move_to(const struct device *dev, const int32_t micro_steps)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->move_to == NULL) {
		return -ENOSYS;
	}
	return api->move_to(dev, micro_steps);
}

/**
 * @brief Run the stepper with a given step interval in a given direction
 *
 * @details The stepper shall be set into motion and run continuously until
 * stalled or stopped using some other command, for instance, stepper_stop(). This
 * function is non-blocking.
 *
 * @param dev pointer to the stepper driver instance
 * @param direction The direction to set
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_run(const struct device *dev, enum stepper_direction direction);

static inline int z_impl_stepper_run(const struct device *dev,
				     const enum stepper_direction direction)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->run == NULL) {
		return -ENOSYS;
	}
	return api->run(dev, direction);
}

/**
 * @brief Stop the stepper
 * @details Cancel all active movements, however keep the coils energized.
 *
 * @param dev pointer to the stepper driver instance
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_stop(const struct device *dev);

static inline int z_impl_stepper_stop(const struct device *dev)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->stop == NULL) {
		return -ENOSYS;
	}
	return api->stop(dev);
}

/**
 * @brief Check if the stepper is currently moving
 *
 * @param dev pointer to the stepper driver instance
 * @param is_moving Pointer to a boolean to store the moving status of the stepper
 *
 * @retval -EIO General input / output error
 * @retval -ENOSYS If not implemented by device driver
 * @retval 0 Success
 */
__syscall int stepper_is_moving(const struct device *dev, bool *is_moving);

static inline int z_impl_stepper_is_moving(const struct device *dev, bool *is_moving)
{
	const struct stepper_driver_api *api = (const struct stepper_driver_api *)dev->api;

	if (api->is_moving == NULL) {
		return -ENOSYS;
	}
	return api->is_moving(dev, is_moving);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#include <zephyr/syscalls/stepper.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_STEPPER_H_ */
