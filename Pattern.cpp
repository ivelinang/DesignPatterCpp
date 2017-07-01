#include <iostream>;
//#include "Vector.h";
#include <string>;
#include <list>;
#include <vector>;
//#include <map>;
//#include <thread>;
//#include <numeric>;
//#include <future>;
//#include <algorithm>;
//#include <accumulate>
//include others...

using namespace std;

//enum
//Direction
enum class Direction {North, South, East, West};

//Abstract main class
class MapSite {
public:
	virtual void Enter() = 0;
};

//Room subclass
class Room : public MapSite {
public:
	Room(int roomNo) {
		_roomNumber = roomNo;
	};

	MapSite* getSide(Direction dir) const;
	void setSide(Direction dir, MapSite* mp) {
	};

	virtual void Enter() {
	};
private:
	MapSite* _sides[4];
	int _roomNumber;
};

//Wall subclass
class Wall :public MapSite {
public:
	Wall() {};
	Wall(const Wall& other){}

	virtual void Enter() {};
	Wall* Clone() {
		return new Wall(*this);
	};
};

//Door subclass
class Door :public MapSite {
public:
	Door(Room* rm = 0, Room* rm2 = 0) {
		_room1 = rm;
		_room2 = rm2;
	};

	virtual void Enter() {};
	Room* OtherSideFrom(Room* rm) {};

	Door(const Door& other) {
		_room1 = other._room1;
		_room2 = other._room2;
	};

	void Initialize(Room* r1, Room* r2) {
		_room1 = r1;
		_room2 = r2;
	};

	Door* Clone() const {
		return new Door(*this);
	};

private:
	Room* _room1;
	Room* _room2;
	bool _isOPen;
};

//new class Maze - collection of rooms
class Maze {
public:
	Maze() {};

	void AddRoom(Room* rm) {
		rooms.push_back(rm);
	}
	Room* RoomNo(int no) const {}; //means Room cannot be changed once set, no change of member variable	. The method will not modify non-mutable member variable of the class
private:
	vector<Room*> rooms;
};

//----------------------------------Abstract Factory------------------------------------------


//now using Abstract Factory

//abstract class MazeFactory

class MazeFactory {
public:
	MazeFactory() {};

	virtual Maze* MakeMaze() const {
		return new Maze;
	}

	virtual Wall* MakeWall() const {
		return new Wall;
	}

	virtual Room* MakeRoom(int n) const {
		return new Room(n);
	}

	virtual Door* MakeDoor(Room* r1, Room* r2) const {
		return new Door(r1, r2);
	}
};

//----------------------------------Builder------------------------------------------


class MazeBuilder {
public:
	virtual void BuildMaze() {};
	virtual void BuildRoom(int room) {};
	virtual void BuildDoor(int roomFrom, int roomTo) {};

	virtual Maze* getMaze() = 0; //return the maze to the client. Subclasses will override to return the mazes that they built.
protected:
	MazeBuilder() {};
};

//------------------------------------------Prototype-----------------------------------
class MazePrototypeFactory :public MazeFactory {
public:
	MazePrototypeFactory(Maze*, Wall*, Room*, Door*);

	virtual Maze* MakeMaze() const {
		return new Maze;
	};
	virtual Room* MakeRoom(int r) const {
		return new Room(r);
	};
	virtual Wall* MakeWall() const;
	virtual Door* MakeDoor(Room* r1, Room* r2) const;

private:
	Maze* _prototypeMaze;
	Room* _prototypeRoom;
	Wall* _prototypeWall;
	Door* _prototypeDoor;
};

MazePrototypeFactory::MazePrototypeFactory(Maze* m, Wall* w, Room* r, Door* d) {
	_prototypeMaze = m;
	_prototypeWall = w;
	_prototypeRoom = r;
	_prototypeDoor = d;
}

Wall* MazePrototypeFactory::MakeWall() const {
	return _prototypeWall->Clone();
};

Door* MazePrototypeFactory::MakeDoor(Room* r1, Room* r2) const {
	Door* door = _prototypeDoor->Clone();
	door->Initialize(r1, r2);
	return door;
};


//---------------------------all together-------------------------------------

//create the Maze for that game
class MazeGame : public Maze {
public:
	MazeGame() {};

	Maze* CreateMaze() ;
	Maze* CreateMaze(MazeFactory& f) ;
	Maze* CreateMaze(MazeBuilder& builder);
};

//using no Abstract Factory
Maze* MazeGame::CreateMaze() {
	Maze* aMaze = new Maze;
	Room* r1 = new Room(1);
	Room* r2 = new Room(2);
	Door* theDoor = new Door(r1, r2);

	aMaze->AddRoom(r1);
	aMaze->AddRoom(r2);

	r1->setSide(Direction::North, new Wall);
	r1->setSide(Direction::North, theDoor);
	//etc...

	return aMaze;
}

//now the new version of CreateMaze
Maze* MazeGame::CreateMaze(MazeFactory& factory) {

	Maze* aMaze = factory.MakeMaze();
	Room* r1 = factory.MakeRoom(1);
	Room* r2 = factory.MakeRoom(2);
	Door* aDoor = factory.MakeDoor(r1, r2);

	aMaze->AddRoom(r1);
	aMaze->AddRoom(r2);

	r1->setSide(Direction::North, factory.MakeWall());
	r1->setSide(Direction::East, aDoor);

	return aMaze;

}

//we can create another factory for enchanted maze
class EnchantedMazeFactory : public MazeFactory {
public:
	EnchantedMazeFactory() {};

	//... etc with EnchantedRooms
	//create enchantedRooms
	//create walls with bombs
	//obviously need to create such object first
	//override the methods MakeRoom, makeWall
};

//now if you want to create enchanted Maze, simply call CreateMaze with the Enchanted Maze



//----------------------------------Builder------------------------------------------



//use Builder in CreateMaze

Maze* MazeGame::CreateMaze(MazeBuilder& builder) {
	builder.BuildMaze();
	builder.BuildRoom(1);
	builder.BuildRoom(2);
	builder.BuildDoor(1, 2);

	return builder.getMaze();
}

class StandardMazeBuilder : public MazeBuilder {
public:
	StandardMazeBuilder();

	virtual void BuildMaze() ;
	virtual void BuildRoom(int) ;
	virtual void BuildDoor(int, int) {};

	virtual Maze* getMaze();

private:
	Direction CommonWall(Room*, Room*);
	Maze* _currentMaze;
};

StandardMazeBuilder::StandardMazeBuilder() {
	_currentMaze = 0;
}

Maze* StandardMazeBuilder::getMaze() {
	return _currentMaze;
}

void StandardMazeBuilder::BuildMaze() {
	_currentMaze = new Maze;
}

void StandardMazeBuilder::BuildRoom(int n) {
	Room* room = new Room(n);
	_currentMaze->AddRoom(room);

	room->setSide(Direction::North, new Wall);
}

//-----------------------------------------Singleton-------------------------

class Singleton {
public:
	static Singleton* Instance();
protected:
	Singleton() {};
private:
	static Singleton* _instance;
};

//implementation
Singleton* Singleton::_instance = 0;

Singleton* Singleton::Instance() {
	if (_instance == 0) {
		_instance = new Singleton;
	}
	return _instance;
}

//----------------------------------Adapter----------------------
//adapter is used to bridge two classes that seem difficult to combine at first sight
//it uses multiple inheritance, and tries to adapt the methods to fit both classes

class Coord {
public: 
	Coord() {};
};

class Point {
public:
	Point(Coord* c1, Coord* c2) {
		_cord1 = c1;
		_cord2 = c2;
	}
private:
	Coord* _cord1;
	Coord* _cord2;
};

class Manipulator {
public:
	Manipulator() {};
};

//first class
class Shape {
public:
	Shape() {};

	virtual void BoundingBox(
		Point& bottomleft, Point& topRight) const {};
	
	virtual Manipulator* CreateManipulator() const {};
};

class TextView {
public:
	TextView() {};

	void GetOrigin(Coord& x, Coord& y) const {};
	void GetExtent(Coord& width, Coord& height) const {};
};

//inherit from both
//key is one brach for interface
//another for implementation
class TextShape : public Shape, private TextView {
public:
	TextShape() {};

	void BoundingBox(Point& bottomleft, Point& topRight) {
		//
	}

};

//another way - better. Use pointer to the second class
class TextShape2 :public Shape {
public:
	TextShape2(TextView*);
private:
	TextView* _text;
};

//ok
//Adapter is used after initial code set up
//we have old interface, we need to turn it into  a new one

typedef int Coordinate;
typedef int Dimension;

// Desired interface
class Rectangle
{
public:
	virtual void draw() = 0;
};

// Legacy component
class LegacyRectangle
{
public:
	LegacyRectangle(Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2)
	{
		x1_ = x1;
		y1_ = y1;
		x2_ = x2;
		y2_ = y2;
		cout << "LegacyRectangle:  create.  (" << x1_ << "," << y1_ << ") => ("
			<< x2_ << "," << y2_ << ")" << endl;
	}
	void oldDraw()
	{
		cout << "LegacyRectangle:  oldDraw.  (" << x1_ << "," << y1_ <<
			") => (" << x2_ << "," << y2_ << ")" << endl;
	}
private:
	Coordinate x1_;
	Coordinate y1_;
	Coordinate x2_;
	Coordinate y2_;
};

// Adapter wrapper
class RectangleAdapter : public Rectangle, private LegacyRectangle
{
public: //this is where we adapt from old to new
	RectangleAdapter(Coordinate x, Coordinate y, Dimension w, Dimension h) :
		LegacyRectangle(x, y, x + w, y + h)
	{
		cout << "RectangleAdapter: create.  (" << x << "," << y <<
			"), width = " << w << ", height = " << h << endl;
	}
	virtual void draw()
	{
		cout << "RectangleAdapter: draw." << endl;
		oldDraw();
	}
};

//-----------------------------------------Composite-----------------------------

//Important is child-parent relationship
//objects that hold many other related objects

// 2. Create an "interface" (lowest common denominator)
class Component
{
public:
	virtual void traverse() = 0;
};

class Leaf : public Component
{
	// 1. Scalar class   3. "isa" relationship
	int value;
public:
	Leaf(int val)
	{
		value = val;
	}
	void traverse()
	{
		cout << value << ' ';
	}
};

class Composite : public Component
{
	// 1. Vector class   3. "isa" relationship
	vector < Component * > children; // 4. "container" coupled to the interface
public:
	// 4. "container" class coupled to the interface
	void add(Component *ele)
	{
		children.push_back(ele);
	}
	void traverse()
	{
		for (int i = 0; i < children.size(); i++)
			// 5. Use polymorphism to delegate to children
			children[i]->traverse();
	}
};

//------------------------------------------Decorator---------------------------
//you can create new functionality at run time
//this is opposed to multiple inheritance, which is static at run time
//but with decorator user can create all combinations of functionality that he wants

//abstract main class
class VisualComponent {
public:
	VisualComponent() {};

	virtual void Draw() = 0;
	virtual void Resize() = 0;

};

//implementation
class VisualConcrete:public VisualComponent {
public:
	VisualConcrete() {};

	void Draw() {};
	void Resize() {};
};

//abstract decorator

class Decorator : public VisualComponent {
public:
	Decorator(VisualComponent* v) {
		_component = v;
	};

	void Draw() {
		_component->Draw();
	};

	void Resize() {
		_component->Resize();
	};

private:
	VisualComponent* _component;
};

//define specific decoration
class BorderDecorator :public Decorator {
public:
	BorderDecorator(VisualComponent* v, int borderWidth):Decorator(v) {
		_width = borderWidth;
		//Decorator::Decorator(v);
	};

	void Draw() {
		Decorator::Draw();
		DrawBorder(_width);
	}

	void DrawBorder(int) {};

private:
	int _width;
};

void test_decorator() {
	VisualConcrete* vc= new VisualConcrete();
	BorderDecorator bd(vc, 3);
	bd.Draw();
	delete vc;
}

//AnotherTest-----------------------------------------------------------------

class I {
public:
	virtual ~I() {}
	virtual void do_it() = 0;
};

class A : public I {
public:
	~A() {
		cout << "A dtor" << '\n';
		cin.get();
	}
	/*virtual*/
	void do_it() {
		cout << 'A';
		cin.get();
	}
};

class D : public I {
public:
	D(I *inner) {
		m_wrappee = inner;
	}
	~D() {
		delete m_wrappee;
	}
	/*virtual*/
	void do_it() {
		m_wrappee->do_it();
	}
private:
	I *m_wrappee;
};

class X : public D {
public:
	X(I *core) : D(core) {}
	~X() {
		cout << "X dtor" << "   ";
		cin.get();
	}
	/*virtual*/
	void do_it() {
		D::do_it();
		cout << 'X';
		cin.get();
	}
};

class Y : public D {
public:
	Y(I *core) : D(core) {}
	~Y() {
		cout << "Y dtor" << "   ";
		cin.get();
	}
	/*virtual*/
	void do_it() {
		D::do_it();
		cout << 'Y';
		cin.get();
	}
};

class Z : public D {
public:
	Z(I *core) : D(core) {}
	~Z() {
		cout << "Z dtor" << "   ";
		cin.get();
	}
	/*virtual*/
	void do_it() {
		D::do_it();
		cout << 'Z';
		cin.get();
	}
};

//---------------------------------Facade--------------------------------
//provide one single interface to a complex system with many objects interfaces

//----------------------------------------Flyweight------------------------------
//combine many similar objects into one interface

class Gazillion
{
public:
	Gazillion(int value_one)
	{
		m_value_one = value_one;
		cout << "ctor: " << m_value_one << '\n';
		cin.get();
	}
	~Gazillion()
	{
		cout << m_value_one << ' ';
		cin.get();
	}
	void report(int value_two)
	{
		cout << m_value_one << value_two << ' ';
		cin.get();
	}
private:
	int m_value_one;
};

class Factory
{
public:
	static Gazillion *get_fly(int in)
	{
		if (!s_pool[in])
			s_pool[in] = new Gazillion(in);
		return s_pool[in];
	}
	static void clean_up()
	{
		cout << "dtors: ";
		for (int i = 0; i < X; ++i)
			if (s_pool[i])
				delete s_pool[i];
		cout << '\n';
	}
	static int X, Y;
private:
	static Gazillion *s_pool[];
};

int Factory::X = 6, Factory::Y = 10;
Gazillion *Factory::s_pool[] =
{
	0, 0, 0, 0, 0, 0
};

void test_flyweight()
{
	for (int i = 0; i < Factory::X; ++i)
	{
		for (int j = 0; j < Factory::Y; ++j)
			Factory::get_fly(i)->report(j);
		cout << '\n';
	}
	Factory::clean_up();
}

//--------------------Proxy-------------------

class RealImage
{
	int m_id;
public:
	RealImage(int i)
	{
		m_id = i;
		cout << "   $$ ctor: " << m_id << '\n';
	}
	~RealImage()
	{
		cout << "   dtor: " << m_id << '\n';
	}
	void draw()
	{
		cout << "   drawing image " << m_id << '\n';
	}
};

// 1. Design an "extra level of indirection" wrapper class
class Image
{
	// 2. The wrapper class holds a pointer to the real class
	RealImage *m_the_real_thing;
	int m_id;
	static int s_next;
public:
	Image()
	{
		m_id = s_next++;
		// 3. Initialized to null
		m_the_real_thing = 0;
	}
	~Image()
	{
		delete m_the_real_thing;
	}
	void draw()
	{
		// 4. When a request comes in, the real object is
		//    created "on first use"
		if (!m_the_real_thing)
			m_the_real_thing = new RealImage(m_id);
		// 5. The request is always delegated
		m_the_real_thing->draw();
	}
};
int Image::s_next = 1;

void test_proxy() {

	Image images[5];

	for (int i; true;)
	{
		cout << "Exit[0], Image[1-5]: ";
		cin >> i;
		if (i == 0)
			break;
		images[i - 1].draw();
	}
}

//image2 old version
class Image2
{
	int m_id;
	static int s_next;
public:
	Image2()
	{
		m_id = s_next++;
		cout << "   $$ ctor: " << m_id << '\n';
	}
	~Image2()
	{
		cout << "   dtor: " << m_id << '\n';
	}
	void draw()
	{
		cout << "   drawing image " << m_id << '\n';
	}
};
int Image2::s_next = 1;

void test_image2() {
	Image2 images[5];

	for (int i; true;)
	{
		cout << "Exit[0], Image[1-5]: ";
		cin >> i;
		if (i == 0)
			break;
		images[i - 1].draw();
	}
}

void test_new_decorator() {
	I *anX = new X(new A);
	I *anXY = new Y(new X(new A));
	I *anXYZ = new Z(new Y(new X(new A)));
	anX->do_it();
	cout << '\n';
	anXY->do_it();
	cout << '\n';
	anXYZ->do_it();
	cout << '\n';
	cin.get();
	delete anX;
	delete anXY;
	delete anXYZ;
}

//----------------------------Chain of Responsibility----------------------------
//Handle->Handle->Handle-....->Base

class Base
{
	Base *next; // 1. "next" pointer in the base class
public:
	Base()
	{
		next = 0;
	}
	void setNext(Base *n)
	{
		next = n;
	}
	void add(Base *n)
	{
		if (next)
			next->add(n);
		else
			next = n;
	}
	// 2. The "chain" method in the base class always delegates to the next obj
	virtual void handle(int i)
	{
		next->handle(i);
	}
};

class Handler1 : public Base
{
public:
	/*virtual*/void handle(int i)
	{
		if (rand() % 3)
		{
			// 3. Don't handle requests 3 times out of 4
			cout << "H1 passed " << i << "  "; cin.get();

			Base::handle(i); // 3. Delegate to the base class
		}
		else
			cout << "H1 handled " << i << "  "; cin.get();
	}
};

class Handler2 : public Base
{
public:
	/*virtual*/void handle(int i)
	{
		if (rand() % 3)
		{
			cout << "H2 passed " << i << "  "; cin.get();
			Base::handle(i);
		}
		else
			cout << "H2 handled " << i << "  "; cin.get();
	}
};

class Handler3 : public Base
{
public:
	/*virtual*/void handle(int i)
	{
		if (rand() % 3)
		{
			cout << "H3 passed " << i << "  "; cin.get();
			Base::handle(i);
		}
		else
			cout << "H3 handled " << i << "  "; cin.get();

	};
};

void test_handle() {
	Handler1 root;
	Handler2 two;
	Handler3 thr;
	root.add(&two);
	root.add(&thr);
	thr.setNext(&root);
	for (int i = 1; i < 10; i++)
	{
		root.handle(i);
		cout << '\n';
	}
}

class HelpHandler {
public:
	HelpHandler(HelpHandler* s) : _successor(s) {};
	void HandleHelp() {
		if (_successor) {
			_successor->HandleHelp();
		}
	}

private:
	HelpHandler* _successor;
};

//---------------------------Iterator---------------------------
//clients can access internal members of objects w/out exposing the representation

//template interface
template <class item>
class Iterator {
public:
	virtual int First() = 0;
	virtual int Next() = 0;
	virtual bool IsDone() const = 0;
	virtual item CurrentItem() const = 0;
protected:
	//Iterator();


};

//implementation
template <class item>
class ListIterator : public Iterator<item> {
public:
	ListIterator(const vector<item>* aList);
	/*virtual*/ int First();
	/*virtual*/ int Next();
	/*virtual*/ bool IsDone() const;
	/*virtual*/ item CurrentItem() const;

private:
	const vector<item>* _list;
	int _current;
};

//implement the methods
template <class item>
ListIterator<item>::ListIterator(const vector<item>* aList) : _list(aList), _current(0) {};

template <class item>
int ListIterator<item>::First() {
	_current = 0;
	return _current;
};

template <class item>
int ListIterator<item>::Next() {
	_current++;
	return _current;
};

template <class item>
bool ListIterator<item>::IsDone() const {
	return _current >= _list->size();
};


template <class item>
item ListIterator<item>::CurrentItem() const {
	if (IsDone()) {
		throw out_of_range("");
	}
	//auto it = _list->begin();
	//advance(it, _current);
	//item x = it;
	return _list->at(_current);
};

void test_iterator() {
	vector<int> intList = { 1,2,3,4,5 };
	//intList.
	vector<int>* intListPointer = &intList;
	vector<int> *p = new vector<int>;
	ListIterator<int> iter(intListPointer);
	int k= iter.First();

	for (iter.First(); !iter.IsDone(); iter.Next()) {
		cout << iter.CurrentItem();

	}
	cin.get();
}


//---------------------------------Mediator---------------------------------
//Mediator is an object that defines how a set of objects interact with each other
//#define _CRT_SECURE_NO_WARNINGS // for strcpy function

class FileSelectionDialog;

//Abstract class
class Widget
{
public:
	Widget(FileSelectionDialog *mediator, char *name)
	{
		_mediator = mediator;
		_name[0] = *name;
	}
	virtual void changed();
	virtual void updateWidget() = 0;
	virtual void queryWidget() = 0;
protected:
	char _name[20];
private:
	FileSelectionDialog *_mediator;
};

class List : public Widget
{
public:
	List(FileSelectionDialog *dir, char *name) : Widget(dir, name) {}
	void queryWidget()
	{
		cout << "   " << _name << " list queried" << endl;
	}
	void updateWidget()
	{
		cout << "   " << _name << " list updated" << endl;
	}
};

class Edit : public Widget
{
public:
	Edit(FileSelectionDialog *dir, char *name) : Widget(dir, name) {}
	void queryWidget()
	{
		cout << "   " << _name << " edit queried" << endl;
	}
	void updateWidget()
	{
		cout << "   " << _name << " edit updated" << endl;
	}
};

class FileSelectionDialog
{
public:
	enum Widgets
	{
		FilterEdit, DirList, FileList, SelectionEdit
	};
	FileSelectionDialog()
	{
		_components[FilterEdit] = new Edit(this, "filter");
		_components[DirList] = new List(this, "dir");
		_components[FileList] = new List(this, "file");
		_components[SelectionEdit] = new Edit(this, "selection");
	}
	virtual ~FileSelectionDialog();
	void handleEvent(int which)
	{
		_components[which]->changed();
	}
	virtual void widgetChanged(Widget *theChangedWidget)
	{
		if (theChangedWidget == _components[FilterEdit])
		{
			_components[FilterEdit]->queryWidget();
			_components[DirList]->updateWidget();
			_components[FileList]->updateWidget();
			_components[SelectionEdit]->updateWidget();
		}
		else if (theChangedWidget == _components[DirList])
		{
			_components[DirList]->queryWidget();
			_components[FileList]->updateWidget();
			_components[FilterEdit]->updateWidget();
			_components[SelectionEdit]->updateWidget();
		}
		else if (theChangedWidget == _components[FileList])
		{
			_components[FileList]->queryWidget();
			_components[SelectionEdit]->updateWidget();
		}
		else if (theChangedWidget == _components[SelectionEdit])
		{
			_components[SelectionEdit]->queryWidget();
			cout << "   file opened" << endl;
		}
	}
private:
	Widget *_components[4];
};

FileSelectionDialog::~FileSelectionDialog()
{
	for (int i = 0; i < 4; i++)
		delete _components[i];
}

void Widget::changed()
{
	_mediator->widgetChanged(this);
}

void test_mediator()
{
	FileSelectionDialog fileDialog;
	int i;

	cout << "Exit[0], Filter[1], Dir[2], File[3], Selection[4]: ";
	cin >> i;

	while (i)
	{
		fileDialog.handleEvent(i - 1);
		cout << "Exit[0], Filter[1], Dir[2], File[3], Selection[4]: ";
		cin >> i;
	}
	cin.get();
};

//------------------------------------Memento------------------------------
//
//Without violating encapsulation, capture and externalize an object's internal state so that the object can be returned to this state later.
//A magic cookie that encapsulates a "check point" capability.
//Promote undo or rollback to full object status.




void test_Maze() {
	MazeGame game  = MazeGame();
	EnchantedMazeFactory factory = EnchantedMazeFactory();
	//game.CreateMaze(factory);
	Maze* maze = game.CreateMaze(factory);

	StandardMazeBuilder builder;
	game.CreateMaze(builder);
	maze = builder.getMaze();

	//prototype
	//MazeGame game;
	MazePrototypeFactory simplePrototypeFactor(new Maze, new Wall, new Room(1), new Door);
	Maze* maze2 = game.CreateMaze(simplePrototypeFactor);

	
}


int main() {

	//test_new_decorator();
	//cin.get();
	//test_flyweight();
	//test_proxy();
	//test_image2();
	//test_handle();
	test_iterator();
	test_mediator();

	/*
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 10; ++j) {
			cout << i<<" "<< j; 			
			cin.get();
		}
			

	}
	*/

	return 0;
}