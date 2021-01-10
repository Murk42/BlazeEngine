    #include "BlazeEngine/Console/Frame.h"
#include "BlazeEngine/Console/Console.h"
#include <PDC/curses.h>

namespace Blaze
{
	namespace Console
	{
        size_t Frame::Apply(const char* ptr)
        {
            switch (*ptr)
            {
            case 'f':
                color = (color & 0b111000) | ((*(ptr + 1) - '0') & 0b000111);
                wattron((WINDOW*)this->ptr, COLOR_PAIR(color));
                return 2;
                break;
            case 'b':
                color = (color & 0b000111) | (((*(ptr + 1) - '0') << 3) & 0b000111);
                wattron((WINDOW*)this->ptr, COLOR_PAIR(color));
                return 2;
                break;
            case 'a':
                switch (*(ptr + 1))
                {
                case 'u':
                    wattron((WINDOW*)this->ptr, A_UNDERLINE);
                    break;
                case 'b':
                    wattron((WINDOW*)this->ptr, A_BOLD);
                    break;
                case 'r':
                    wattron((WINDOW*)this->ptr, A_REVERSE);
                    break;
                case 'd':
                    wattroff((WINDOW*)this->ptr, A_UNDERLINE);
                    wattroff((WINDOW*)this->ptr, A_BOLD);
                    wattroff((WINDOW*)this->ptr, A_REVERSE);
                    break;
                }
                return 2;
                break;
            case '$':
                return 0;
                break;
            }
            return 0;
        }
        void Frame::WriteRaw(const char* ptr, size_t count)
        {
            size_t _count = count;
            Vec2i cur(0, cursor.y);
            if (count == 0 || cursor.x + count < 0 || cursor.x > size.x)
                return;
            else if (cursor.x < 0)
            {
                cur.x = 0;
                ptr -= cursor.x;
                count += cursor.x;
            }
            else if (cursor.x + int(count) < size.x)
                cur.x = cursor.x;
            else if (cursor.x < size.x)
            {
                cur.x = cursor.x;
                count = size_t(size.x) - size_t(cursor.x);
            }
            wmove((WINDOW*)this->ptr, cur.y + 1, cur.x + 1);
            waddnstr((WINDOW*)this->ptr, ptr, (int)count);
            cursor.x += (int)_count;
            wmove((WINDOW*)this->ptr, cursor.y + 1, cursor.x + 1);
        }

        Frame::Frame(const Vec2i& pos, const Vec2i& size)
            : ptr(newwin(size.y + 2, size.x + 2, pos.y, pos.x)), pos(pos), size(size), color(0)
        {
            //if (ptr == nullptr)
            //    throw "newwin returned nullptr. Maybe the requested size of the frame is too big";
            Refresh();
        }
        Frame::Frame(Frame&& f) noexcept
            : ptr(std::exchange(f.ptr, nullptr)), pos(f.pos), size(f.size), color(f.color)
        {

        }
        Frame::~Frame()
        {
            if (ptr != nullptr)
                delwin((WINDOW*)ptr);
        }

        void Frame::Write(const String& s, const Vec2i& pos)
        {
            MoveCursor(pos);
            Write(s);
        }
        void Frame::Write(const String& s)
        {
            Write(s.Ptr(), s.Size());
        }
        void Frame::Write(const char* ptr, size_t count)
        {
            const char* begin = ptr;
            const char* end = ptr;
            const char* const last = ptr + count;
            while (end != last)
            {
                if (*end == '$')
                {
                    WriteRaw(begin, end - begin);
                    end += Apply(end + 1) + 1;
                    begin = end;
                }
                else
                    ++end;
            }
            WriteRaw(begin, end - begin);
        }

        void Frame::MoveCursor(const Vec2i& pos)
        {
            cursor = pos;
            wmove((WINDOW*)ptr, cursor.y + 1, cursor.x + 1);
        }

        void Frame::Clear()
        {
            cursor = Vec2i(0);
            wclear((WINDOW*)ptr);
            box((WINDOW*)ptr, 0, 0);
        }
        void Frame::Refresh()
        {
            Apply("f7");
            Apply("b0");
            Apply("ad");
            box((WINDOW*)ptr, 0, 0);
            wrefresh((WINDOW*)ptr);
        }   

        void Frame::operator=(Frame&& f) noexcept
        {
            ptr = std::exchange(f.ptr, nullptr);
            pos = f.pos;
            size = f.size;
            color = f.color;
        }
	}
}