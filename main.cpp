#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <functional>

#include "StudentManager.h"
#include "reports.h"
#include "grade_logic.h"
#include "file_manager.h"

using namespace std;
using namespace sf;

namespace Pal
{
Color bg        {15,  17,  23};
Color sidebar   {22,  26,  35};
Color card      {28,  34,  46};
Color accent    {99, 179, 237};
Color accentDim {52,  93, 125};
Color success   {72, 199, 142};
Color danger    {252,110,110};
Color warning   {250,202, 86};
Color textHi    {220,230,245};
Color textMid   {140,155,180};
Color textLow   { 70, 82,100};
Color divider   { 38, 46, 62};
Color inputBg   { 18, 22, 32};
Color inputBord { 55, 68, 90};
Color hoverBtn  { 40, 48, 66};
}

static Text makeText(const string& str, Font& font, unsigned size, Color col)
{
    Text t(str, font, size);
    t.setFillColor(col);
    return t;
}

static void drawRoundRect(RenderWindow& win, Vector2f pos, Vector2f sz, Color fill, float radius = 6.f)
{
    RectangleShape center({sz.x, sz.y - 2*radius});
    center.setPosition(pos.x, pos.y + radius);
    center.setFillColor(fill);
    win.draw(center);
    RectangleShape top({sz.x - 2*radius, radius});
    top.setPosition(pos.x + radius, pos.y);
    top.setFillColor(fill);
    win.draw(top);
    RectangleShape bot({sz.x - 2*radius, radius});
    bot.setPosition(pos.x + radius, pos.y + sz.y - radius);
    bot.setFillColor(fill);
    win.draw(bot);
    CircleShape corner(radius);
    corner.setFillColor(fill);
    corner.setPosition(pos.x, pos.y);
    win.draw(corner);
    corner.setPosition(pos.x + sz.x - 2*radius, pos.y);
    win.draw(corner);
    corner.setPosition(pos.x, pos.y + sz.y - 2*radius);
    win.draw(corner);
    corner.setPosition(pos.x + sz.x - 2*radius, pos.y + sz.y - 2*radius);
    win.draw(corner);
}

static void drawLine(RenderWindow& win, Vector2f a, Vector2f b, Color col)
{
    Vertex line[2] = {Vertex(a,col), Vertex(b,col)};
    win.draw(line, 2, Lines);
}

struct SideBtn
{
    FloatRect bounds;
    string label;
    string icon;
    bool active  = false;
    bool hovered = false;
    SideBtn(const string& ic, const string& lbl, float y) : label(lbl), icon(ic)
    {
        bounds = {10.f, y, 220.f, 42.f};
    }
    void update(Vector2f mp, bool isActive)
    {
        active = isActive;
        hovered = bounds.contains(mp);
    }
    void draw(RenderWindow& win, Font& font)
    {
        Color bg = active ? Pal::accentDim : hovered ? Pal::hoverBtn : Color::Transparent;
        if (bg != Color::Transparent) drawRoundRect(win, {bounds.left, bounds.top}, {bounds.width, bounds.height}, bg, 5.f);
        if (active)
        {
            RectangleShape bar({3.f, bounds.height - 10.f});
            bar.setPosition(bounds.left, bounds.top + 5.f);
            bar.setFillColor(Pal::accent);
            win.draw(bar);
        }
        auto ic = makeText(icon, font, 15, active ? Pal::accent : Pal::textMid);
        ic.setPosition(bounds.left + 12.f, bounds.top + (bounds.height - ic.getGlobalBounds().height)/2.f - 2.f);
        win.draw(ic);
        auto lbl = makeText(label, font, 14, active ? Pal::textHi : Pal::textMid);
        lbl.setPosition(bounds.left + 34.f, bounds.top + (bounds.height - lbl.getGlobalBounds().height)/2.f - 2.f);
        win.draw(lbl);
    }
};

struct InputField
{
    FloatRect bounds;
    string value;
    string placeholder;
    bool focused = false;
    bool numeric = false;
    float blinkTimer = 0.f;
    bool cursorVis = true;
    InputField() {}
    InputField(const string& ph, FloatRect b, bool num = false) : placeholder(ph), bounds(b), numeric(num) {}
    void handleEvent(Event& ev)
    {
        if (!focused) return;
        if (ev.type == Event::TextEntered)
        {
            char c = static_cast<char>(ev.text.unicode);
            if (ev.text.unicode == 8 && !value.empty()) value.pop_back();
            else if (ev.text.unicode >= 32 && ev.text.unicode < 127)
            {
                if (numeric && !isdigit(c) && c != '.') return;
                value += c;
            }
        }
    }
    void update(float dt, Vector2f mp, bool clicked)
    {
        if (clicked) focused = bounds.contains(mp);
        blinkTimer += dt;
        if (blinkTimer > 0.5f)
        {
            blinkTimer = 0.f;
            cursorVis = !cursorVis;
        }
    }
    void draw(RenderWindow& win, Font& font, const string& labelTxt = "")
    {
        if (!labelTxt.empty())
        {
            auto lbl = makeText(labelTxt, font, 12, Pal::textMid);
            lbl.setPosition(bounds.left, bounds.top - 18.f);
            win.draw(lbl);
        }
        RectangleShape box({bounds.width, bounds.height});
        box.setPosition(bounds.left, bounds.top);
        box.setFillColor(Pal::inputBg);
        box.setOutlineColor(focused ? Pal::accent : Pal::inputBord);
        box.setOutlineThickness(1.f);
        win.draw(box);
        string disp = value.empty() ? placeholder : value;
        Color dcol = value.empty() ? Pal::textLow : Pal::textHi;
        string shown = disp;
        if (focused && cursorVis) shown += "|";
        auto txt = makeText(shown, font, 14, dcol);
        txt.setPosition(bounds.left + 10.f, bounds.top + (bounds.height - txt.getGlobalBounds().height)/2.f - 2.f);
        win.draw(txt);
    }
    void clear()
    {
        value.clear();
        focused = false;
    }
    float toFloat() const
    {
        try
        {
            return stof(value);
        }
        catch(...)
        {
            return -1.f;
        }
    }
    int toInt() const
    {
        try
        {
            return stoi(value);
        }
        catch(...)
        {
            return -1;
        }
    }
};

struct ActionBtn
{
    FloatRect bounds;
    string label;
    Color color;
    bool hovered = false;
    ActionBtn() {}
    ActionBtn(const string& lbl, FloatRect b, Color col = Pal::accent) : label(lbl), bounds(b), color(col) {}
    void update(Vector2f mp)
    {
        hovered = bounds.contains(mp);
    }
    bool wasClicked(Vector2f mp, bool click)
    {
        return click && bounds.contains(mp);
    }
    void draw(RenderWindow& win, Font& font)
    {
        Color bg = hovered ? Color(color.r, color.g, color.b, 210) : Color(color.r, color.g, color.b, 160);
        drawRoundRect(win, {bounds.left, bounds.top}, {bounds.width, bounds.height}, bg, 5.f);
        auto txt = makeText(label, font, 14, Pal::textHi);
        txt.setPosition(bounds.left + (bounds.width - txt.getGlobalBounds().width)/2.f, bounds.top + (bounds.height - txt.getGlobalBounds().height)/2.f - 2.f);
        win.draw(txt);
    }
};

struct Toast
{
    string msg;
    Color color = Pal::success;
    float timer = 0.f;
    float life = 3.f;
    bool active = false;
    void show(const string& m, Color c = Pal::success)
    {
        msg = m;
        color = c;
        timer = 0.f;
        active = true;
    }
    void update(float dt)
    {
        if (active)
        {
            timer += dt;
            if(timer>life) active=false;
        }
    }
    void draw(RenderWindow& win, Font& font, float winW, float winH)
    {
        if (!active) return;
        float alpha = (timer > life - 0.5f) ? (life - timer) / 0.5f : 1.f;
        Color bg = color;
        bg.a = static_cast<Uint8>(200 * alpha);
        float w = 400.f, h = 44.f, x = (winW - w) / 2.f, y = winH - 70.f;
        drawRoundRect(win, {x,y}, {w,h}, bg, 6.f);
        auto txt = makeText(msg, font, 14, Color(255,255,255, static_cast<Uint8>(255*alpha)));
        txt.setPosition(x + (w - txt.getGlobalBounds().width)/2.f, y + (h - txt.getGlobalBounds().height)/2.f - 2.f);
        win.draw(txt);
    }
};

struct ScrollPanel
{
    vector<pair<string,Color>> lines;
    float scrollY = 0.f;
    float lineH = 20.f;
    FloatRect bounds;
    void setBounds(FloatRect b)
    {
        bounds = b;
    }
    void clear()
    {
        lines.clear();
        scrollY = 0.f;
    }
    void add(const string& s, Color c = Pal::textHi)
    {
        lines.push_back({s,c});
    }
    void scroll(float delta)
    {
        scrollY = max(0.f, scrollY - delta * 30.f);
    }
    void draw(RenderWindow& win, Font& font)
    {
        View panelView(FloatRect(bounds.left, bounds.top + scrollY, bounds.width, bounds.height));
        panelView.setViewport(FloatRect(bounds.left / win.getSize().x, bounds.top / win.getSize().y, bounds.width / win.getSize().x, bounds.height / win.getSize().y));
        win.setView(panelView);
        float y = bounds.top + 6.f;
        for (auto& [s, c] : lines)
        {
            auto t = makeText(s, font, 13, c);
            t.setPosition(bounds.left + 8.f, y);
            win.draw(t);
            y += lineH;
        }
        win.setView(win.getDefaultView());
    }
};

struct GradeListBuilder
{
    vector<InputField> fields;
    ActionBtn addBtn;
    float startY, startX, fieldW;
    void init(float x, float y, float w)
    {
        startX = x;
        startY = y;
        fieldW = w;
        addBtn = ActionBtn("+ Add Grade", {x, y, 120.f, 30.f}, Pal::accentDim);
        fields.clear();
    }
    void addField()
    {
        if (fields.size() >= 10) return;
        float fy = startY + 38.f + (float)fields.size() * 38.f;
        fields.push_back(InputField("Grade " + to_string(fields.size()+1), {startX, fy, fieldW, 32.f}, true));
        addBtn.bounds.top = fy + 40.f;
    }
    vector<float> getGrades(string& err)
    {
        vector<float> grades;
        for (auto& f : fields)
        {
            float v = f.toFloat();
            if (v < 0.f || v > 100.f)
            {
                err = "grades must be 0-100";
                return {};
            }
            grades.push_back(v);
        }
        if (grades.empty())
        {
            err = "add a grade";
            return {};
        }
        return grades;
    }
    void handleEvent(Event& ev)
    {
        for (auto& f : fields) f.handleEvent(ev);
    }
    void update(float dt, Vector2f mp, bool click)
    {
        for (auto& f : fields) f.update(dt, mp, click);
        addBtn.update(mp);
        if (addBtn.wasClicked(mp, click)) addField();
    }
    void draw(RenderWindow& win, Font& font)
    {
        auto hdr = makeText("grades:", font, 12, Pal::textMid);
        hdr.setPosition(startX, startY + 4.f);
        win.draw(hdr);
        addBtn.draw(win, font);
        for (auto& f : fields) f.draw(win, font);
    }
    void clear()
    {
        fields.clear();
        addBtn.bounds.top = startY + 38.f;
    }
};

enum class Page { Dashboard, AddStudent, RemoveStudent, ViewAll, SearchID, SearchName, UpdateGrades, Reports };

int main()
{
    loadFromFile();
    const unsigned WIN_W = 1180, WIN_H = 780;
    const float SIDEBAR_W = 240.f, CONTENT_X = SIDEBAR_W + 10.f, CONTENT_Y = 20.f;
    RenderWindow window(VideoMode(WIN_W, WIN_H), "Student Management System", Style::Titlebar | Style::Close);
    window.setFramerateLimit(60);
    Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    vector<SideBtn> sideButtons = { {"#", "Dashboard", 80.f}, {"+", "Add Student", 135.f}, {"-", "Remove Student", 188.f}, {"=", "View All", 241.f}, {"?", "Search ID", 294.f}, {"@", "Search Name", 347.f}, {"~", "Update Grades", 400.f}, {"%", "Reports", 453.f} };
    Page currentPage = Page::Dashboard;

    InputField fldID("Enter ID", {CONTENT_X+20, 120, 220, 36}, true), fldName("Enter Name", {CONTENT_X+20, 180, 300, 36}, false), fldIDSrch("Enter ID", {CONTENT_X+20, 120, 220, 36}, true), fldNameSrch("Search name", {CONTENT_X+20,120,300,36}, false), fldIDRem("Student ID", {CONTENT_X+20, 120, 220, 36}, true), fldIDUpd("Student ID", {CONTENT_X+20, 120, 220, 36}, true);
    GradeListBuilder gradeBuilderAdd, gradeBuilderUpd;
    gradeBuilderAdd.init(CONTENT_X+20, 230.f, 280.f);
    gradeBuilderUpd.init(CONTENT_X+20, 180.f, 280.f);

    ActionBtn btnSubmitAdd("Add Student", {CONTENT_X+20, 560, 160, 38}, Pal::success), btnSubmitRem("Remove Student", {CONTENT_X+20, 200, 180, 38}, Pal::danger), btnSubmitSrchID("Search", {CONTENT_X+20, 180, 120, 36}, Pal::accent), btnSubmitSrchNm("Search", {CONTENT_X+20, 180, 120, 36}, Pal::accent), btnSubmitUpd("Update Grades", {CONTENT_X+20, 560, 160, 38}, Pal::warning);
    vector<ActionBtn> reportBtns = { ActionBtn("Class Average", {CONTENT_X+20, 90, 200, 36}, Pal::accent), ActionBtn("Top Student", {CONTENT_X+20, 140, 200, 36}, Pal::success), ActionBtn("Lowest Student", {CONTENT_X+20, 190, 200, 36}, Pal::danger), ActionBtn("Pass/Fail", {CONTENT_X+20, 240, 200, 36}, Pal::warning), ActionBtn("Ranked Students", {CONTENT_X+20, 290, 200, 36}, Pal::accentDim) };

    ScrollPanel panel, tablePanel;
    panel.setBounds({CONTENT_X, CONTENT_Y + 340.f, WIN_W - CONTENT_X - 10.f, WIN_H - CONTENT_Y - 360.f});
    tablePanel.setBounds({CONTENT_X, CONTENT_Y + 40.f, WIN_W - CONTENT_X - 10.f, WIN_H - CONTENT_Y - 60.f});
    Toast toast;
    Clock clock;

    auto captureOutput = [&](function<void()> fn) -> string { ostringstream oss; streambuf* oldBuf = cout.rdbuf(oss.rdbuf()); fn(); cout.rdbuf(oldBuf); return oss.str(); };
    auto populatePanel = [&](ScrollPanel& p, const string& raw, Color col = Pal::textHi)
    {
        p.clear();
        istringstream ss(raw);
        string line;
        while (getline(ss, line)) p.add(line, col);
    };

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        bool mouseClickedThisFrame = false;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                saveToFile();
                window.close();
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) mouseClickedThisFrame = true;
            if (event.type == Event::MouseWheelScrolled)
            {
                panel.scroll(event.mouseWheelScroll.delta);
                tablePanel.scroll(event.mouseWheelScroll.delta);
            }
            fldID.handleEvent(event);
            fldName.handleEvent(event);
            fldIDSrch.handleEvent(event);
            fldNameSrch.handleEvent(event);
            fldIDRem.handleEvent(event);
            fldIDUpd.handleEvent(event);
            gradeBuilderAdd.handleEvent(event);
            gradeBuilderUpd.handleEvent(event);
        }

        for (int i = 0; i < (int)sideButtons.size(); i++)
        {
            sideButtons[i].update(mousePos, currentPage == (Page)i);
            if (mouseClickedThisFrame && sideButtons[i].bounds.contains(mousePos))
            {
                currentPage = (Page)i;
                panel.clear();
                tablePanel.clear();
                gradeBuilderAdd.clear();
                gradeBuilderUpd.clear();
                gradeBuilderAdd.addField();
                gradeBuilderUpd.addField();
            }
        }

        fldID.update(dt, mousePos, mouseClickedThisFrame);
        fldName.update(dt, mousePos, mouseClickedThisFrame);
        fldIDSrch.update(dt, mousePos, mouseClickedThisFrame);
        fldNameSrch.update(dt, mousePos, mouseClickedThisFrame);
        fldIDRem.update(dt, mousePos, mouseClickedThisFrame);
        fldIDUpd.update(dt, mousePos, mouseClickedThisFrame);
        gradeBuilderAdd.update(dt, mousePos, mouseClickedThisFrame);
        gradeBuilderUpd.update(dt, mousePos, mouseClickedThisFrame);
        btnSubmitAdd.update(mousePos);
        btnSubmitRem.update(mousePos);
        btnSubmitSrchID.update(mousePos);
        btnSubmitSrchNm.update(mousePos);
        btnSubmitUpd.update(mousePos);
        for (auto& b : reportBtns) b.update(mousePos);
        toast.update(dt);

        if (currentPage == Page::AddStudent && btnSubmitAdd.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldID.toInt();
            string name = fldName.value, err;
            auto grades = gradeBuilderAdd.getGrades(err);
            if (id <= 0) toast.show("invalid id", Pal::danger);
            else if (name.empty()) toast.show("empty name", Pal::danger);
            else if (!err.empty()) toast.show(err, Pal::danger);
            else
            {
                addStudent(id, name, grades);
                toast.show("added student", Pal::success);
                fldID.clear();
                fldName.clear();
                gradeBuilderAdd.clear();
                gradeBuilderAdd.addField();
            }
        }

        if (currentPage == Page::RemoveStudent && btnSubmitRem.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDRem.toInt();
            if (id <= 0) toast.show("invalid id", Pal::danger);
            else
            {
                string out = captureOutput([&] { removeStudent(id); });
                toast.show(out.empty() ? "removed" : out, Pal::warning);
                fldIDRem.clear();
            }
        }

        if (currentPage == Page::SearchID && btnSubmitSrchID.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDSrch.toInt();
            if (id <= 0) toast.show("invalid id", Pal::danger);
            else
            {
                string out = captureOutput([&] { displayStudentByID(id); });
                populatePanel(panel, out);
                if (out.empty()) toast.show("not found", Pal::warning);
            }
        }

        if (currentPage == Page::SearchName && btnSubmitSrchNm.wasClicked(mousePos, mouseClickedThisFrame))
        {
            if (fldNameSrch.value.empty()) toast.show("enter name", Pal::danger);
            else
            {
                string out = captureOutput([&] { searchStudentByName(fldNameSrch.value); });
                populatePanel(panel, out);
                if (out.empty()) toast.show("not found", Pal::warning);
            }
        }

        if (currentPage == Page::UpdateGrades && btnSubmitUpd.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDUpd.toInt();
            string err;
            auto grades = gradeBuilderUpd.getGrades(err);
            if (id <= 0) toast.show("invalid id", Pal::danger);
            else if (!err.empty()) toast.show(err, Pal::danger);
            else
            {
                updateStudentGrades(id, grades);
                toast.show("updated", Pal::success);
                fldIDUpd.clear();
                gradeBuilderUpd.clear();
                gradeBuilderUpd.addField();
            }
        }

        if (currentPage == Page::ViewAll && tablePanel.lines.empty()) populatePanel(tablePanel, captureOutput([&] { displayAllStudentsTable(); }));

        if (currentPage == Page::Reports)
        {
            for (int i = 0; i < 5; i++)
            {
                if (reportBtns[i].wasClicked(mousePos, mouseClickedThisFrame))
                {
                    string out;
                    Color c = (i==1)?Pal::success:(i==2)?Pal::danger:(i==3)?Pal::warning:Pal::accent;
                    switch(i)
                    {
                    case 0:
                        out = captureOutput([&] {displayClassAverage();});
                        break;
                    case 1:
                        out = captureOutput([&] {displayTopStudent();});
                        break;
                    case 2:
                        out = captureOutput([&] {displayLowestStudent();});
                        break;
                    case 3:
                        out = captureOutput([&] {displayPassFailSummary();});
                        break;
                    case 4:
                        out = captureOutput([&] {displayRankedStudents();});
                        break;
                    }
                    populatePanel(panel, out, c);
                }
            }
        }

        window.clear(Pal::bg);
        RectangleShape sidebar({SIDEBAR_W, (float)WIN_H});
        sidebar.setFillColor(Pal::sidebar);
        window.draw(sidebar);
        auto sysTitle = makeText("SMS", font, 28, Pal::accent);
        sysTitle.setPosition(20.f, 20.f);
        window.draw(sysTitle);
        drawLine(window, {10.f, 74.f}, {SIDEBAR_W-10.f, 74.f}, Pal::divider);
        for (auto& btn : sideButtons) btn.draw(window, font);
        drawLine(window, {10.f, 510.f}, {SIDEBAR_W-10.f, 510.f}, Pal::divider);

        ActionBtn btnSaveSide("Save", {15.f, 520.f, 100.f, 34.f}, Pal::accentDim), btnExitSide("Exit", {125.f, 520.f, 100.f, 34.f}, Color(80,40,40));
        btnSaveSide.update(mousePos);
        btnExitSide.update(mousePos);
        btnSaveSide.draw(window, font);
        btnExitSide.draw(window, font);
        if (btnSaveSide.wasClicked(mousePos, mouseClickedThisFrame))
        {
            saveToFile();
            toast.show("saved", Pal::success);
        }
        if (btnExitSide.wasClicked(mousePos, mouseClickedThisFrame))
        {
            saveToFile();
            window.close();
        }
        drawLine(window, {SIDEBAR_W, 0}, {SIDEBAR_W, (float)WIN_H}, Pal::divider);

        auto pageTitle = [&](const string& t, const string& sub = "")
        {
            auto tt = makeText(t, font, 22, Pal::textHi);
            tt.setPosition(CONTENT_X + 20.f, CONTENT_Y);
            window.draw(tt);
            if (!sub.empty())
            {
                auto st = makeText(sub, font, 12, Pal::textMid);
                st.setPosition(CONTENT_X + 20.f, CONTENT_Y + 30.f);
                window.draw(st);
            }
            drawLine(window, {CONTENT_X, CONTENT_Y + 48.f}, {(float)WIN_W - 10.f, CONTENT_Y + 48.f}, Pal::divider);
        };

        if (currentPage == Page::Dashboard)
        {
            pageTitle("Dashboard", "system overview");
            float cx = CONTENT_X + 20.f, cy = CONTENT_Y + 70.f, cw = 190.f, ch = 90.f, gap = 16.f;
            string labels[4] = {"Add Data", "View Reports", "ID Search", "Manage"};
            Color colors[4] = {Pal::accent, Pal::success, Pal::warning, Pal::danger};
            for (int i = 0; i < 4; i++)
            {
                float x = cx + (cw + gap) * i;
                drawRoundRect(window, {x, cy}, {cw, ch}, Pal::card, 8.f);
                RectangleShape top({cw, 4.f});
                top.setPosition(x, cy);
                top.setFillColor(colors[i]);
                window.draw(top);
                auto lbl = makeText(labels[i], font, 12, Pal::textMid);
                lbl.setPosition(x + 12.f, cy + 14.f);
                window.draw(lbl);
            }
            auto g1 = makeText("quick guide:", font, 14, Pal::accent);
            g1.setPosition(cx, cy + 120.f);
            window.draw(g1);
            auto g2 = makeText("- use sidebar to navigate\n- add students with unique id\n- update grades to replace data\n- check reports for analysis", font, 13, Pal::textMid);
            g2.setPosition(cx, cy + 145.f);
            window.draw(g2);
            auto warn = makeText("important: click 'save' or 'exit' to store changes before closing", font, 13, Pal::warning);
            warn.setPosition(cx, cy + 240.f);
            window.draw(warn);
        }
        else if (currentPage == Page::AddStudent)
        {
            pageTitle("Add Student");
            fldID.draw(window, font, "id");
            fldName.bounds.top = 185.f;
            fldName.draw(window, font, "name");
            float lastY = 240.f + 38.f + (float)gradeBuilderAdd.fields.size() * 38.f;
            gradeBuilderAdd.startY = 240.f;
            gradeBuilderAdd.addBtn.bounds.top = lastY;
            btnSubmitAdd.bounds.top = lastY + 50.f;
            gradeBuilderAdd.draw(window, font);
            btnSubmitAdd.draw(window, font);
        }
        else if (currentPage == Page::RemoveStudent)
        {
            pageTitle("Remove Student");
            fldIDRem.draw(window, font, "id");
            btnSubmitRem.bounds.top = 175.f;
            btnSubmitRem.draw(window, font);
        }
        else if (currentPage == Page::ViewAll)
        {
            pageTitle("All Students");
            tablePanel.bounds = {CONTENT_X, CONTENT_Y + 60.f, WIN_W - CONTENT_X - 10.f, WIN_H - CONTENT_Y - 80.f};
            tablePanel.draw(window, font);
        }
        else if (currentPage == Page::SearchID)
        {
            pageTitle("Search ID");
            fldIDSrch.draw(window, font, "id");
            btnSubmitSrchID.draw(window, font);
            panel.bounds = {CONTENT_X, 240.f, WIN_W - CONTENT_X - 10.f, WIN_H - 260.f};
            panel.draw(window, font);
        }
        else if (currentPage == Page::SearchName)
        {
            pageTitle("Search Name");
            fldNameSrch.draw(window, font, "name");
            btnSubmitSrchNm.draw(window, font);
            panel.bounds = {CONTENT_X, 240.f, WIN_W - CONTENT_X - 10.f, WIN_H - 260.f};
            panel.draw(window, font);
        }
        else if (currentPage == Page::UpdateGrades)
        {
            pageTitle("Update Grades");
            fldIDUpd.draw(window, font, "id");
            float lastY = 155.f + 38.f + (float)gradeBuilderUpd.fields.size() * 38.f;
            gradeBuilderUpd.startY = 155.f;
            gradeBuilderUpd.addBtn.bounds.top = lastY;
            btnSubmitUpd.bounds.top = lastY + 50.f;
            gradeBuilderUpd.draw(window, font);
            btnSubmitUpd.draw(window, font);
        }
        else if (currentPage == Page::Reports)
        {
            pageTitle("Reports", "different report types available");
            for (auto& b : reportBtns) b.draw(window, font);
            panel.bounds = {CONTENT_X, 368.f, WIN_W - CONTENT_X - 10.f, WIN_H - 388.f};
            panel.draw(window, font);
        }

        toast.draw(window, font, (float)WIN_W, (float)WIN_H);
        window.display();
    }
    return 0;
}
