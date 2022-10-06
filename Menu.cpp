#include "pch.hpp"
#include "Menu.hpp"

BF::MenuItem::MenuItem(const string& str, const int t)
{
	name = str;
	type = t;
}

void BF::Action::Excute() const
{
	func();
}

BF::Action::Action(const string& str, const function <void()>& fun) : MenuItem(str, Action_t)
{
	func = fun;
}

const bool& BF::Toggle::IsOn() const
{
	return on;
}

bool BF::Toggle::toggle()
{
	on = !on;
	return on;
}

void BF::Toggle::Excute()
{
	func(*this);
}

BF::Toggle::Toggle(const string& str, const function <void(Toggle&)>& fun): MenuItem(str, Toggle_t)
{
	func = fun;
	on   = false;
	Excute();
}

void BF::Menu::clear()
{
	items.clear();
}

void BF::Menu::add_action(const string& str, const function <void()>& fun)
{
	const auto x = static_pointer_cast <MenuItem>(make_shared <Action>(str, fun));
	items.push_back(x);
}

void BF::Menu::add_toggle(const string& str, const function <void(Toggle&)>& fun)
{
	const auto x = static_pointer_cast <MenuItem>(make_shared <Toggle>(str, fun));
	items.push_back(x);
}

shared_ptr <BF::Submenu> BF::Menu::add_submenu(const string& str, const function <void()>& fun)
{
	const auto x = make_shared <Submenu>(str, fun);
	items.push_back(static_pointer_cast <MenuItem>(x));
	return x;
}

BF::Menu::Menu(const string& str, const int t) : MenuItem(str, Menu_t)
{
	cur_item = 0;
}

BF::Submenu::Submenu(const string& str, const function <void()>& fun) : Menu(str, Submenu_t)
{
	func = fun;
}

BF::MenuTab::MenuTab(const shared_ptr <Menu> m)
{
	menu = m;
}

void BF::InitMenu(vector <shared_ptr <MenuTab> >& tabs, const BF::Settings& settings)
{
	shared_ptr <Menu> t1(new Menu("主菜单"));
	tabs.push_back(make_shared <MenuTab>(t1));
	tabs.back()->menu_stack.push(shared_ptr(t1));
	shared_ptr <Menu> t2(new Menu("次要菜单"));
	tabs.push_back(make_shared <MenuTab>(t2));
	tabs.back()->menu_stack.push(shared_ptr(t2));
	const auto m1 = t1->add_submenu("1");
	const auto m2 = t1->add_submenu("2");
	const auto m3 = t1->add_submenu("3");
	const auto m4 = m1->add_submenu("11");
	m1->add_action
		("11", []
		{
			cout << "11" << endl;
		});
	m1->add_toggle
		("t1", [](Toggle& toggle)
		{
			if (toggle.IsOn())
			{
				cout << "toggled!" << endl;
				toggle.toggle();
			}
		});

	const auto m5 = m2->add_submenu("21");
	const auto m6 = m5->add_submenu("51");
	m2->clear();
}
