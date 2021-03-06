/****************************************************************************
 *
 *   Copyright (C) 2014 PX4 Development Team. All rights reserved.
 *   Author: Pavel Kirienko <pavel.kirienko@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "indication_controller.hpp"
#include <uavcan/equipment/indication/LightsCommand.hpp>
#include <uavcan/equipment/indication/BeepCommand.hpp>
#include <config/config.h>
#include <motor/motor.h>
#include <led.h>

namespace uavcan_node
{
namespace
{

unsigned self_light_index = 0;

CONFIG_PARAM_INT("uavcan_light_index", 0,   0,  255)


void cb_light_command(const uavcan::ReceivedDataStructure<uavcan::equipment::indication::LightsCommand>& msg)
{
	for (auto& cmd : msg.commands)
	{
		if (cmd.light_id == self_light_index)
		{
			led_set_rgb(cmd.color.red / 31.F, cmd.color.green / 63.F, cmd.color.blue / 31.F);
			break;
		}
	}
}

void cb_beep_command(const uavcan::ReceivedDataStructure<uavcan::equipment::indication::BeepCommand>& msg)
{
	motor_beep(static_cast<int>(msg.frequency), static_cast<int>(msg.duration * 1000));
}

}

int init_indication_controller(uavcan::INode& node)
{
	static uavcan::Subscriber<uavcan::equipment::indication::LightsCommand> sub_light(node);
	static uavcan::Subscriber<uavcan::equipment::indication::BeepCommand> sub_beep(node);

	self_light_index = config_get("uavcan_light_index");

	int res = 0;

	res = sub_light.start(cb_light_command);
	if (res != 0) {
		return res;
	}

	res = sub_beep.start(cb_beep_command);
	if (res != 0) {
		return res;
	}

	return 0;
}

}
