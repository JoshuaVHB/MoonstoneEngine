#pragma once

#include <bitset>
#include <queue>


/*https://www.youtube.com/watch?v=h7HCdEyGRRw&list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD&index=9*/

class Keyboard {

public:

	static unsigned char letterCodeFromChar(const char c) {
		return 0x41 + (c-'a');
	}

	class Event 
	{
	public:
		enum class Type {PRESS,RELEASE, INVALID};
		Event()									noexcept : m_type(Type::INVALID), m_code(0)  {}
		Event(Type type, unsigned char code)	noexcept : m_type(type), m_code(code) {}

		[[nodiscard]] bool isPress()			const noexcept	{ return m_type == Type::PRESS;		}
		[[nodiscard]] bool isRelease()			const noexcept	{ return m_type == Type::RELEASE;	}
		[[nodiscard]] bool isInvalide()			const noexcept	{ return m_type == Type::INVALID;	}
		[[nodiscard]] unsigned char getCode()	const noexcept	{ return m_code;					}
	private:

		Type m_type;
		unsigned char m_code;


	};

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;


	enum {KEY_COUNT=256u};
	bool isKeyPressed(const char keyCode) const noexcept { return m_keyStates[keyCode]; }
	Event readKey() {
		if (m_keyBuffer.size() == 0) return Keyboard::Event();
		Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	bool keyIsEmpty() const { return m_keyBuffer.empty(); }
	bool flushKey() { m_keyBuffer = std::queue<Event>(); }

	char readChar() {
		if (m_charBuffer.size() == 0) return 0;
		char c = m_charBuffer.front();
		m_charBuffer.pop();
		return c;
	}

	bool charIsEmpty() const { return m_charBuffer.empty(); }
	void flushChar() {
		m_charBuffer = std::queue<char>();
	}
	void flush() {
		flushKey();
		flushChar();
	}

	void enableAutorepeat()				noexcept { m_autorepeat = true; }
	void disableAutorepeat()			noexcept { m_autorepeat = false; }
	bool autorepeatIsEnable()	const	noexcept { return m_autorepeat == true; }

private:

	std::bitset<KEY_COUNT> m_keyStates;
	std::queue<Event> m_keyBuffer;
	std::queue<char> m_charBuffer;

	bool m_autorepeat = false;
public:
	void onKeyPressed(unsigned char code) noexcept {
		m_keyStates[code] = true;
		m_keyBuffer.push(Keyboard::Event{ Event::Type::PRESS, code });
		trimBuffer(m_keyBuffer);
	}

	void onKeyReleased(unsigned char code) noexcept {
		m_keyStates[code] = false;
		m_keyBuffer.push(Keyboard::Event{ Event::Type::RELEASE, code });
		trimBuffer(m_keyBuffer);
	}

	void onChar(char c) noexcept {
		m_charBuffer.push(c);
		trimBuffer(m_charBuffer);
	}

	void clearStates() { m_keyStates.reset(); }

	template<typename _T> 
	void trimBuffer(std::queue<_T>& buffer)
	{
		while (buffer.size() > 16) buffer.pop();
	}


};

class Mouse {

public:

	class Event
	{
	public:
		enum class Type { 
			LPRESS, LRELEASE,
			RPRESS, RRELEASE,
			WHEELUP, WHEELDOWN,
			MOVE, INVALID };

		Event()									noexcept : m_type(Type::INVALID) {}
		Event(Type type, const Mouse& parent)	noexcept 
			: m_type(type), x(parent.m_x), y(parent.m_y),
			  isLeftPressed(parent.m_isLeftPressed), isRightPressed(parent.m_isRightPressed)
		{}

		[[nodiscard]] Type getType()			const noexcept { return m_type ; }
		[[nodiscard]] bool isValid()			const noexcept { return m_type != Type::INVALID; }
		[[nodiscard]] std::pair<int, int>	getPos()			const noexcept { return { x,y }; }
		[[nodiscard]] int					getPosX()			const noexcept { return x; }
		[[nodiscard]] int					getPosY()			const noexcept { return y; }
		[[nodiscard]] bool					leftIsPressed()		const noexcept { return isLeftPressed; }
		[[nodiscard]] bool					rightIsPressed()	const noexcept { return isRightPressed; }
	private:

		Type m_type;
		int x{}, y{};
		bool isLeftPressed=false, isRightPressed=false;


	};


	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	[[nodiscard]] std::pair<int, int>	getPos()			const noexcept { return { m_x,m_y };		}
	[[nodiscard]] int					getPosX()			const noexcept { return m_x;				}
	[[nodiscard]] int					getPosY()			const noexcept { return m_y;				}
	[[nodiscard]] bool					isLeftPressed()		const noexcept { return m_isLeftPressed;	}
	[[nodiscard]] bool					isRightPressed()	const noexcept { return m_isRightPressed;	}
	[[nodiscard]] bool					isEmpty()			const noexcept { return m_buffer.empty(); }
	[[nodiscard]] Event					read()					noexcept { 
		if (isEmpty()) return Mouse::Event();
		Event& e = m_buffer.front();
		return (m_buffer.pop(), e);
	}

	void flush() noexcept{ m_buffer = std::queue<Event>(); }

private:

	static constexpr unsigned short buffSize = 16;
	std::queue<Event> m_buffer;
	int m_x, m_y;
	bool m_isLeftPressed, m_isRightPressed;
public:

	void onMouseMoved(int x, int y)		{ m_x = x; m_y = y; m_buffer.push(Event(Event::Type::MOVE, *this)); trimBuffer(); }
	void onLeftPress	(int x, int y)	{ m_isLeftPressed  = true; m_buffer.push(Event(Event::Type::LPRESS, *this)); trimBuffer(); }
	void onLeftRelease(int x, int y)	{ m_isLeftPressed  = false; m_buffer.push(Event(Event::Type::LRELEASE, *this)); trimBuffer(); }
	void onRightPress(int x, int y)		{ m_isRightPressed	= true; m_buffer.push(Event(Event::Type::RPRESS, *this)); trimBuffer(); }
	void onRightRelease(int x, int y)	{ m_isRightPressed	= false; m_buffer.push(Event(Event::Type::RRELEASE, *this)); trimBuffer(); }
	void onWheelUp		(int x, int y)	{ m_buffer.push(Event(Event::Type::WHEELUP, *this)); trimBuffer();}
	void onWheelDown	(int x, int y)	{ m_buffer.push(Event(Event::Type::WHEELDOWN, *this)); trimBuffer();}
	void trimBuffer		(){}


};

class InputsManager
{



};