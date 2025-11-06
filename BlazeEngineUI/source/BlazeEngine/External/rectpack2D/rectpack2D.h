#pragma once

#include "BlazeEngine/External/rectpack2D/finders_interface.h"

constexpr bool allow_flip = false;
using spaces_type = rectpack2D::empty_spaces<allow_flip, rectpack2D::default_empty_spaces>;
using rect_type = rectpack2D::output_rect_t<spaces_type>;

rectpack2D::rect_wh PackRects(std::vector<rect_type>& rectangles)
{
	const auto runtime_flipping_mode = rectpack2D::flipping_option::DISABLED;
	const auto max_side = 4096;
	const auto discard_step = 1;

	auto report_successful = [](rect_type&) {
		return rectpack2D::callback_result::CONTINUE_PACKING;
	};

	auto report_unsuccessful = [](rect_type&) {
		return rectpack2D::callback_result::ABORT_PACKING;
	};

	const auto result_size = rectpack2D::find_best_packing<spaces_type>(
		rectangles,
		make_finder_input(
			max_side,
			discard_step,
			report_successful,
			report_unsuccessful,
			runtime_flipping_mode
		));

	return result_size;
}