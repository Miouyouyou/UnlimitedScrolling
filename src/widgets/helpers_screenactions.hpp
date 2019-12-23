#pragma once

#include <cstdint>

namespace myy {
	template <typename ActionTypeEnum>
	struct ClickAction {
		typedef bool (*action_ptr)
			(void * user_ptr, int16_t, int16_t, ActionTypeEnum);
		action_ptr action;
		void * user_ptr;
		static inline bool dummy_action(
			void * const user_ptr,
			int16_t const x,
			int16_t const y,
			ActionTypeEnum action)
		{
			LOG(
				"[DUMMY ACTION]\n"
				"\tI'm a dummy action that should not have been called.\n"
				"\tPtr : %p\n"
				"\tCoords : {x = %d, y = %d}"
				"\tAction : %d\n",
				user_ptr, x, y, (int)action);
			return false;
		}

		ClickAction() :
			action(dummy_action),
			user_ptr(this)
		{}
		ClickAction(
			action_ptr oaction,
			void * ouser_ptr) :
			action(oaction),
			user_ptr(ouser_ptr)
		{}

		bool execute(
			int16_t const x, int16_t const y, ActionTypeEnum action)
		{
			return action(user_ptr, x, y, action);
		}
		bool execute(Point2D<int16_t> const point, ActionTypeEnum action)
		{
			return action(user_ptr, point.x, point.y, action);
		}
		
	};

	template <int N, typename ActionTypeEnum>
	struct ScreenActionsFixed {
		using Action = ClickAction<ActionTypeEnum>;
		typedef Area<Point2D<int16_t>> Hitbox;

		Hitbox hitboxes[N];
		ClickAction<ActionTypeEnum> actions[N];
		uint32_t last_i;
		ScreenActionsFixed() :
			hitboxes(),
			actions(),
			last_i(0)
		{
		}

		ScreenActionsFixed(
			ScreenActionsFixed & oscreenactions) :
			hitboxes(),
			actions(),
			last_i(0)
		{
			memcpy(
				hitboxes,
				oscreenactions.hitboxes,
				oscreenactions.last_i*sizeof(Hitbox));
			memcpy(
				actions,
				oscreenactions.actions,
				oscreenactions.last_i*sizeof(ClickAction<ActionTypeEnum>));
			last_i = oscreenactions.last_i;
		}

		ScreenActionsFixed(
			ScreenActionsFixed const & oscreenactions) :
			hitboxes(),
			actions(),
			last_i(0)
		{
			memcpy(
				hitboxes,
				oscreenactions.hitboxes,
				oscreenactions.last_i*sizeof(Hitbox));
			memcpy(
				actions,
				oscreenactions.actions,
				oscreenactions.last_i*sizeof(ClickAction<ActionTypeEnum>));
			last_i = oscreenactions.last_i;
		}

		bool deal_with(
			int16_t const x,
			int16_t const y,
			ActionTypeEnum action,
			Point2D<int16_t> const offset)
		{
			Point2D<int16_t> clicked_zone(
				x + offset.x,
				y + offset.y);
			bool dealt = false;
			for (uint32_t i  = 0; (i < last_i) & (!dealt); i++)
				if (hitboxes[i].contains(clicked_zone))
					dealt = actions[i].execute(clicked_zone, action);
			return dealt;
		}

		inline bool deal_with(
			int16_t const x,
			int16_t const y,
			ActionTypeEnum action)
		{
			return deal_with(x, y, action, {0,0});
		}

		inline bool can_add_more() const
		{
			return last_i < N;
		}
		bool add(
			Point2D<int16_t> const upper_left,
			Point2D<int16_t> const bottom_right,
			typename ClickAction<ActionTypeEnum>::action_ptr const action,
			void * user_ptr)
		{
			bool const could_add = can_add_more();
			if (could_add) {
				hitboxes[last_i] = Area(upper_left, bottom_right);
				actions[last_i]  = ClickAction<ActionTypeEnum>(action, user_ptr);
				last_i++;
			}
			return could_add;
		}

		void remove(uint32_t const index) {
			if (index < last_i) {
				uint32_t const after = index+1;
				uint32_t const remaining =
					last_i - after;
				memmove(
					hitboxes+index,
					hitboxes+after,
					sizeof(Hitbox)*remaining);
				memmove(
					actions+index,
					actions+after,
					sizeof(ClickAction<ActionTypeEnum>)*remaining);
				--last_i;
			}
		}
		bool remove(Hitbox hitbox) {
			uint32_t i = 0;
			for (; i < last_i; i++) {
				if (hitboxes[i] == hitbox) {
					remove(i);
					break;
				}
			}
			return i == last_i;
		}
	};

	template <typename ActionTypeEnum>
	struct ScreenActions {
		typedef Area<Point2D<int16_t>> Hitbox;

		dynarray<Hitbox> hitboxes;
		dynarray<ClickAction<ActionTypeEnum>> actions;

		ScreenActions() :
			hitboxes(),
			actions()
		{
		}

		void init() {
			hitboxes.init();
			actions.init();
		}

		ScreenActions(
			ScreenActions & oscreenactions) :
			hitboxes(oscreenactions.hitboxes),
			actions(oscreenactions.actions)
		{}

		ScreenActions(
			ScreenActions const & oscreenactions) :
			hitboxes(oscreenactions.hitboxes),
			actions(oscreenactions.actions)
		{}

		bool deal_with(
			int16_t const x,
			int16_t const y,
			ActionTypeEnum action,
			Point2D<int16_t> const offset)
		{
			Point2D<int16_t> clicked_zone(
				x + offset.x,
				y + offset.y);
			bool dealt = false;
			auto const last_i = hitboxes.length();
			for (uint32_t i  = 0; (i < last_i) & (!dealt); i++)
				if (hitboxes[i].contains(clicked_zone))
					dealt = actions[i].execute(clicked_zone, action);
			return dealt;
		}

		inline bool deal_with(
			int16_t const x,
			int16_t const y,
			ActionTypeEnum action)
		{
			return deal_with(x, y, action, {0,0});
		}

		bool add(
			Point2D<int16_t> const upper_left,
			Point2D<int16_t> const bottom_right,
			typename ClickAction<ActionTypeEnum>::action_ptr const action,
			void * user_ptr)
		{
			Hitbox hbox(upper_left, bottom_right);
			bool action_added = false;
			bool const hitbox_added =
				hitboxes.add_memcpy(&hbox, 1);
			if (hitbox_added) {
				ClickAction clickact(action, user_ptr);
				action_added = actions.add_memcpy(&clickact, 1);
				if (!action_added)
					hitboxes.remove_last();
			}
			return action_added;
		}

		bool remove(Hitbox hitbox) {
			uint32_t i = 0;
			uint32_t const last_i = hitboxes.length();
			for (; i < last_i; i++) {
				if (hitboxes[i] == hitbox) {
					hitboxes.remove_at(i);
					break;
				}
			}
			return i == last_i;
		}
	};
}
