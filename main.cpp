// ============================================================
//  Student Management System - Full SFML GUI
//  Compatible with CodeBlocks + SFML 2.x
//  All operations run entirely inside the window (no terminal)
// ============================================================

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <functional>

// ---- Paste / include your actual headers below ----
#include "StudentManager.h"
#include "reports.h"
#include "grade_logic.h"
#include "file_manager.h"

using namespace std;

// ================================================================
//  COLOUR PALETTE
// ================================================================
namespace Pal
{
    sf::Color bg        {15,  17,  23};   // near-black background
    sf::Color sidebar   {22,  26,  35};   // dark navy sidebar
    sf::Color card      {28,  34,  46};   // slightly lighter card
    sf::Color accent    {99, 179, 237};   // cool sky-blue accent
    sf::Color accentDim {52,  93, 125};   // dimmed accent
    sf::Color success   {72, 199, 142};   // green
    sf::Color danger    {252,110,110};    // red
    sf::Color warning   {250,202, 86};    // yellow
    sf::Color textHi    {220,230,245};    // bright text
    sf::Color textMid   {140,155,180};    // muted text
    sf::Color textLow   { 70, 82,100};    // very muted
    sf::Color divider   { 38, 46, 62};    // subtle line
    sf::Color inputBg   { 18, 22, 32};    // input field bg
    sf::Color inputBord { 55, 68, 90};    // input border idle
    sf::Color hoverBtn  { 40, 48, 66};    // sidebar button hover
}

// ================================================================
//  UTILITIES
// ================================================================
static sf::Text makeText(const std::string& str, sf::Font& font,
                         unsigned size, sf::Color col)
{
    sf::Text t(str, font, size);
    t.setFillColor(col);
    return t;
}

static void drawRoundRect(sf::RenderWindow& win, sf::Vector2f pos,
                          sf::Vector2f sz, sf::Color fill,
                          float radius = 6.f)
{
    // SFML doesn't have built-in rounded rects, so we approximate with
    // one big rect + four circle corners + two thin rects for the ends.
    sf::RectangleShape center({sz.x, sz.y - 2*radius});
    center.setPosition(pos.x, pos.y + radius);
    center.setFillColor(fill);
    win.draw(center);

    sf::RectangleShape top({sz.x - 2*radius, radius});
    top.setPosition(pos.x + radius, pos.y);
    top.setFillColor(fill);
    win.draw(top);

    sf::RectangleShape bot({sz.x - 2*radius, radius});
    bot.setPosition(pos.x + radius, pos.y + sz.y - radius);
    bot.setFillColor(fill);
    win.draw(bot);

    sf::CircleShape corner(radius);
    corner.setFillColor(fill);
    // TL
    corner.setPosition(pos.x, pos.y); win.draw(corner);
    // TR
    corner.setPosition(pos.x + sz.x - 2*radius, pos.y); win.draw(corner);
    // BL
    corner.setPosition(pos.x, pos.y + sz.y - 2*radius); win.draw(corner);
    // BR
    corner.setPosition(pos.x + sz.x - 2*radius, pos.y + sz.y - 2*radius);
    win.draw(corner);
}

static void drawLine(sf::RenderWindow& win, sf::Vector2f a, sf::Vector2f b,
                     sf::Color col)
{
    sf::Vertex line[2] = {sf::Vertex(a,col), sf::Vertex(b,col)};
    win.draw(line, 2, sf::Lines);
}

// ================================================================
//  SIDEBAR BUTTON
// ================================================================
struct SideBtn
{
    sf::FloatRect bounds;
    std::string   label;
    std::string   icon;     // emoji / ASCII icon char
    bool          active  = false;
    bool          hovered = false;

    SideBtn(const std::string& ic, const std::string& lbl, float y)
        : label(lbl), icon(ic)
    {
        bounds = {10.f, y, 220.f, 42.f};
    }

    void update(sf::Vector2f mp, bool isActive)
    {
        active  = isActive;
        hovered = bounds.contains(mp);
    }

    void draw(sf::RenderWindow& win, sf::Font& font)
    {
        sf::Color bg = active  ? Pal::accentDim :
                       hovered ? Pal::hoverBtn   : sf::Color::Transparent;

        if (bg != sf::Color::Transparent)
            drawRoundRect(win, {bounds.left, bounds.top},
                          {bounds.width, bounds.height}, bg, 5.f);

        if (active)
        {
            sf::RectangleShape bar({3.f, bounds.height - 10.f});
            bar.setPosition(bounds.left, bounds.top + 5.f);
            bar.setFillColor(Pal::accent);
            win.draw(bar);
        }

        // icon
        auto ic = makeText(icon, font, 15, active ? Pal::accent : Pal::textMid);
        ic.setPosition(bounds.left + 12.f,
                       bounds.top + (bounds.height - ic.getGlobalBounds().height)/2.f - 2.f);
        win.draw(ic);

        // label
        auto lbl = makeText(label, font, 14,
                            active ? Pal::textHi : Pal::textMid);
        lbl.setPosition(bounds.left + 34.f,
                        bounds.top + (bounds.height - lbl.getGlobalBounds().height)/2.f - 2.f);
        win.draw(lbl);
    }
};

// ================================================================
//  INPUT FIELD
// ================================================================
struct InputField
{
    sf::FloatRect bounds;
    std::string   value;
    std::string   placeholder;
    bool          focused   = false;
    bool          numeric   = false;   // restrict to digits + dot
    float         blinkTimer = 0.f;
    bool          cursorVis  = true;

    InputField() {}
    InputField(const std::string& ph, sf::FloatRect b, bool num = false)
        : placeholder(ph), bounds(b), numeric(num) {}

    void handleEvent(sf::Event& ev)
    {
        if (!focused) return;
        if (ev.type == sf::Event::TextEntered)
        {
            char c = static_cast<char>(ev.text.unicode);
            if (ev.text.unicode == 8 && !value.empty())
                value.pop_back();
            else if (ev.text.unicode >= 32 && ev.text.unicode < 127)
            {
                if (numeric && !std::isdigit(c) && c != '.')
                    return;
                value += c;
            }
        }
    }

    void update(float dt, sf::Vector2f mp, bool clicked)
    {
        if (clicked) focused = bounds.contains(mp);
        blinkTimer += dt;
        if (blinkTimer > 0.5f) { blinkTimer = 0.f; cursorVis = !cursorVis; }
    }

    void draw(sf::RenderWindow& win, sf::Font& font, const std::string& labelTxt = "")
    {
        // label above
        if (!labelTxt.empty())
        {
            auto lbl = makeText(labelTxt, font, 12, Pal::textMid);
            lbl.setPosition(bounds.left, bounds.top - 18.f);
            win.draw(lbl);
        }

        // box
        sf::RectangleShape box({bounds.width, bounds.height});
        box.setPosition(bounds.left, bounds.top);
        box.setFillColor(Pal::inputBg);
        sf::Color bc = focused ? Pal::accent : Pal::inputBord;
        box.setOutlineColor(bc);
        box.setOutlineThickness(1.f);
        win.draw(box);

        // text / placeholder
        std::string disp = value.empty() ? placeholder : value;
        sf::Color   dcol = value.empty() ? Pal::textLow : Pal::textHi;
        std::string shown = disp;
        if (focused && cursorVis && !value.empty())
            shown += "|";
        else if (focused && cursorVis && value.empty())
            shown = "|";

        auto txt = makeText(shown, font, 14, dcol);
        txt.setPosition(bounds.left + 10.f,
                        bounds.top + (bounds.height - txt.getGlobalBounds().height)/2.f - 2.f);
        win.draw(txt);
    }

    void clear() { value.clear(); focused = false; }

    float toFloat() const
    {
        try { return std::stof(value); } catch(...) { return -1.f; }
    }
    int toInt() const
    {
        try { return std::stoi(value); } catch(...) { return -1; }
    }
};

// ================================================================
//  SMALL ACTION BUTTON  (for forms)
// ================================================================
struct ActionBtn
{
    sf::FloatRect bounds;
    std::string   label;
    sf::Color     color;
    bool          hovered = false;

    ActionBtn() {}
    ActionBtn(const std::string& lbl, sf::FloatRect b,
              sf::Color col = Pal::accent)
        : label(lbl), bounds(b), color(col) {}

    void update(sf::Vector2f mp) { hovered = bounds.contains(mp); }

    bool wasClicked(sf::Vector2f mp, bool click)
    { return click && bounds.contains(mp); }

    void draw(sf::RenderWindow& win, sf::Font& font)
    {
        sf::Color bg = hovered ? sf::Color(color.r, color.g, color.b, 210)
                               : sf::Color(color.r, color.g, color.b, 160);
        drawRoundRect(win, {bounds.left, bounds.top},
                      {bounds.width, bounds.height}, bg, 5.f);

        auto txt = makeText(label, font, 14, Pal::textHi);
        float tx = bounds.left + (bounds.width  - txt.getGlobalBounds().width) /2.f;
        float ty = bounds.top  + (bounds.height - txt.getGlobalBounds().height)/2.f - 2.f;
        txt.setPosition(tx, ty);
        win.draw(txt);
    }
};

// ================================================================
//  NOTIFICATION TOAST
// ================================================================
struct Toast
{
    std::string msg;
    sf::Color   color  = Pal::success;
    float       timer  = 0.f;
    float       life   = 3.f;
    bool        active = false;

    void show(const std::string& m, sf::Color c = Pal::success)
    { msg = m; color = c; timer = 0.f; active = true; }

    void update(float dt) { if (active) { timer += dt; if(timer>life) active=false; } }

    void draw(sf::RenderWindow& win, sf::Font& font, float winW, float winH)
    {
        if (!active) return;
        float alpha = 1.f;
        if (timer > life - 0.5f) alpha = (life - timer) / 0.5f;

        sf::Color bg = color;
        bg.a = static_cast<sf::Uint8>(200 * alpha);

        float w = 400.f, h = 44.f;
        float x = (winW - w) / 2.f, y = winH - 70.f;
        drawRoundRect(win, {x,y},{w,h}, bg, 6.f);

        auto txt = makeText(msg, font, 14, sf::Color(255,255,255,
                    static_cast<sf::Uint8>(255*alpha)));
        txt.setPosition(x + (w - txt.getGlobalBounds().width)/2.f,
                        y + (h - txt.getGlobalBounds().height)/2.f - 2.f);
        win.draw(txt);
    }
};

// ================================================================
//  SCROLLABLE TEXT PANEL  (for reports / search results)
// ================================================================
struct ScrollPanel
{
    std::vector<std::pair<std::string,sf::Color>> lines;
    float   scrollY  = 0.f;
    float   lineH    = 20.f;
    sf::FloatRect bounds;

    void setBounds(sf::FloatRect b) { bounds = b; }
    void clear() { lines.clear(); scrollY = 0.f; }
    void add(const std::string& s, sf::Color c = Pal::textHi)
    { lines.push_back({s,c}); }

    void scroll(float delta) { scrollY = std::max(0.f, scrollY - delta * 30.f); }

    void draw(sf::RenderWindow& win, sf::Font& font)
    {
        // clipping via scissor is not directly available in SFML 2;
        // we use a sf::View instead
        float vx = bounds.left / win.getSize().x;
        float vy = bounds.top  / win.getSize().y;
        float vw = bounds.width  / win.getSize().x;
        float vh = bounds.height / win.getSize().y;

        sf::View panelView(sf::FloatRect(bounds.left, bounds.top + scrollY,
                                         bounds.width, bounds.height));
        panelView.setViewport(sf::FloatRect(vx, vy, vw, vh));
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

// ================================================================
//  GRADE LIST BUILDER  (dynamic grade entry inside a form)
// ================================================================
struct GradeListBuilder
{
    std::vector<InputField> fields;
    ActionBtn addBtn;
    float startY;
    float startX;
    float fieldW;

    void init(float x, float y, float w)
    {
        startX = x; startY = y; fieldW = w;
        addBtn = ActionBtn("+ Add Grade",
                           {x, y, 120.f, 30.f}, Pal::accentDim);
        fields.clear();
    }

    void addField()
    {
        if (fields.size() >= 10) return; // max 10 grades
        float fy = startY + 38.f + (float)fields.size() * 38.f;
        std::string ph = "Grade " + std::to_string(fields.size()+1) + " (0-100)";
        fields.push_back(InputField(ph,
            {startX, fy, fieldW, 32.f}, true));
        // move add button down
        addBtn.bounds.top = fy + 40.f;
    }

    std::vector<float> getGrades(std::string& err)
    {
        std::vector<float> grades;
        for (auto& f : fields)
        {
            float v = f.toFloat();
            if (v < 0.f || v > 100.f)
            { err = "Each grade must be 0-100"; return {}; }
            grades.push_back(v);
        }
        if (grades.empty()) { err = "Add at least one grade"; return {}; }
        return grades;
    }

    void handleEvent(sf::Event& ev)
    { for (auto& f : fields) f.handleEvent(ev); }

    void update(float dt, sf::Vector2f mp, bool click)
    {
        for (auto& f : fields) f.update(dt, mp, click);
        addBtn.update(mp);
        if (addBtn.wasClicked(mp, click)) addField();
    }

    void draw(sf::RenderWindow& win, sf::Font& font)
    {
        auto hdr = makeText("Grades:", font, 12, Pal::textMid);
        hdr.setPosition(startX, startY + 4.f);
        win.draw(hdr);

        addBtn.draw(win, font);
        for (size_t i = 0; i < fields.size(); i++)
            fields[i].draw(win, font);
    }

    void clear() { fields.clear(); addBtn.bounds.top = startY + 38.f; }
};

// ================================================================
//  PAGE ENUM
// ================================================================
enum class Page {
    Dashboard, AddStudent, RemoveStudent, ViewAll,
    SearchID, SearchName, UpdateGrades,
    Reports   // covers avg / top / lowest / pass-fail / ranked
};

// ================================================================
//  MAIN
// ================================================================
int main()
{
    loadFromFile();

    const unsigned WIN_W = 1180;
    const unsigned WIN_H = 780;
    const float SIDEBAR_W = 240.f;
    const float CONTENT_X = SIDEBAR_W + 10.f;
    const float CONTENT_Y = 20.f;

    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
                            "Student Management System",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "Could not load arial.ttf – place it beside the exe.\n";
        return -1;
    }

    // ---- Sidebar buttons ----
    std::vector<SideBtn> sideButtons = {
        {"#",  "Dashboard",       80.f},
        {"+",  "Add Student",    135.f},
        {"-",  "Remove Student", 188.f},
        {"=",  "View All",       241.f},
        {"?",  "Search by ID",   294.f},
        {"@",  "Search by Name", 347.f},
        {"~",  "Update Grades",  400.f},
        {"%",  "Reports",        453.f},
    };

    Page currentPage = Page::Dashboard;

    // ---- Input fields (shared / reused per page) ----
    InputField fldID     ("Enter ID",     {CONTENT_X+20, 120, 220, 36}, true);
    InputField fldName   ("Enter Name",   {CONTENT_X+20, 180, 300, 36}, false);
    InputField fldIDSrch ("Enter ID",     {CONTENT_X+20, 120, 220, 36}, true);
    InputField fldNameSrch("Name to search",{CONTENT_X+20,120,300,36}, false);
    InputField fldIDRem  ("Student ID",   {CONTENT_X+20, 120, 220, 36}, true);
    InputField fldIDUpd  ("Student ID",   {CONTENT_X+20, 120, 220, 36}, true);

    GradeListBuilder gradeBuilderAdd, gradeBuilderUpd;
    gradeBuilderAdd.init(CONTENT_X+20, 230.f, 280.f);
    gradeBuilderUpd.init(CONTENT_X+20, 180.f, 280.f);

    // ---- Action buttons (forms) ----
    ActionBtn btnSubmitAdd  ("Add Student",    {CONTENT_X+20, 560, 160, 38}, Pal::success);
    ActionBtn btnSubmitRem  ("Remove Student", {CONTENT_X+20, 200, 180, 38}, Pal::danger);
    ActionBtn btnSubmitSrchID("Search",        {CONTENT_X+20, 180, 120, 36}, Pal::accent);
    ActionBtn btnSubmitSrchNm("Search",        {CONTENT_X+20, 180, 120, 36}, Pal::accent);
    ActionBtn btnSubmitUpd  ("Update Grades",  {CONTENT_X+20, 560, 160, 38}, Pal::warning);
    ActionBtn btnSaveData   ("Save to File",   {CONTENT_X+20, 140, 160, 38}, Pal::accentDim);

    // Report sub-buttons
    std::vector<ActionBtn> reportBtns = {
        ActionBtn("Class Average",   {CONTENT_X+20,  90, 200, 36}, Pal::accent),
        ActionBtn("Top Student",     {CONTENT_X+20, 140, 200, 36}, Pal::success),
        ActionBtn("Lowest Student",  {CONTENT_X+20, 190, 200, 36}, Pal::danger),
        ActionBtn("Pass/Fail",       {CONTENT_X+20, 240, 200, 36}, Pal::warning),
        ActionBtn("Ranked Students", {CONTENT_X+20, 290, 200, 36}, Pal::accentDim),
    };

    // ---- Scroll panel ----
    ScrollPanel panel;
    panel.setBounds({CONTENT_X, CONTENT_Y + 340.f,
                     WIN_W - CONTENT_X - 10.f, WIN_H - CONTENT_Y - 360.f});

    // For View All we use a wider dedicated panel
    ScrollPanel tablePanel;
    tablePanel.setBounds({CONTENT_X, CONTENT_Y + 40.f,
                          WIN_W - CONTENT_X - 10.f, WIN_H - CONTENT_Y - 60.f});

    // ---- Toast ----
    Toast toast;

    // ---- Clock ----
    sf::Clock clock;
    bool      mouseClickedThisFrame = false;
    sf::Vector2f mousePos;

    // Helper: redirect std::cout to a string
    auto captureOutput = [&](std::function<void()> fn) -> std::string
    {
        std::ostringstream oss;
        std::streambuf* oldBuf = std::cout.rdbuf(oss.rdbuf());
        fn();
        std::cout.rdbuf(oldBuf);
        return oss.str();
    };

    // Helper: split string by newline into panel lines
    auto populatePanel = [&](ScrollPanel& p, const std::string& raw,
                              sf::Color col = Pal::textHi)
    {
        p.clear();
        std::istringstream ss(raw);
        std::string line;
        while (std::getline(ss, line)) p.add(line, col);
    };

    // ---- MAIN LOOP ----
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        mouseClickedThisFrame = false;
        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // --- Events ---
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                saveToFile();
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
                mouseClickedThisFrame = true;

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                panel.scroll(event.mouseWheelScroll.delta);
                tablePanel.scroll(event.mouseWheelScroll.delta);
            }

            // Input field events
            fldID.handleEvent(event);
            fldName.handleEvent(event);
            fldIDSrch.handleEvent(event);
            fldNameSrch.handleEvent(event);
            fldIDRem.handleEvent(event);
            fldIDUpd.handleEvent(event);
            gradeBuilderAdd.handleEvent(event);
            gradeBuilderUpd.handleEvent(event);
        }

        // --- Sidebar navigation ---
        for (int i = 0; i < (int)sideButtons.size(); i++)
        {
            sideButtons[i].update(mousePos, currentPage == (Page)i);
            if (mouseClickedThisFrame &&
                sideButtons[i].bounds.contains(mousePos))
            {
                currentPage = (Page)i;
                panel.clear();
                tablePanel.clear();
                // reset grade builders when switching pages
                gradeBuilderAdd.clear();
                gradeBuilderUpd.clear();
                gradeBuilderAdd.addField();
                gradeBuilderUpd.addField();
            }
        }

        // Make sure grade builders start with 1 field if empty
        if (gradeBuilderAdd.fields.empty()) gradeBuilderAdd.addField();
        if (gradeBuilderUpd.fields.empty()) gradeBuilderUpd.addField();

        // --- Update all active widgets ---
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
        btnSaveData.update(mousePos);
        for (auto& b : reportBtns) b.update(mousePos);

        toast.update(dt);

        // ================================================================
        //  PAGE LOGIC  (button actions)
        // ================================================================

        // --- Dashboard: Save button ---
        if (currentPage == Page::Dashboard &&
            btnSaveData.wasClicked(mousePos, mouseClickedThisFrame))
        {
            saveToFile();
            toast.show("Data saved successfully!", Pal::success);
        }

        // --- Add Student ---
        if (currentPage == Page::AddStudent &&
            btnSubmitAdd.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldID.toInt();
            std::string name = fldName.value;
            std::string err;
            auto grades = gradeBuilderAdd.getGrades(err);

            if (id <= 0)
                toast.show("Invalid ID", Pal::danger);
            else if (name.empty())
                toast.show("Name cannot be empty", Pal::danger);
            else if (!err.empty())
                toast.show(err, Pal::danger);
            else
            {
                addStudent(id, name, grades);
                toast.show("Student added!", Pal::success);
                fldID.clear(); fldName.clear();
                gradeBuilderAdd.clear();
                gradeBuilderAdd.addField();
            }
        }

        // --- Remove Student ---
        if (currentPage == Page::RemoveStudent &&
            btnSubmitRem.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDRem.toInt();
            if (id <= 0)
                toast.show("Invalid ID", Pal::danger);
            else
            {
                std::string out = captureOutput([&]{ removeStudent(id); });
                toast.show(out.empty() ? "Done" : out, Pal::warning);
                fldIDRem.clear();
            }
        }

        // --- Search by ID ---
        if (currentPage == Page::SearchID &&
            btnSubmitSrchID.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDSrch.toInt();
            if (id <= 0)
                toast.show("Invalid ID", Pal::danger);
            else
            {
                std::string out = captureOutput([&]{ displayStudentByID(id); });
                populatePanel(panel, out, Pal::textHi);
                if (out.empty()) toast.show("No student found", Pal::warning);
            }
        }

        // --- Search by Name ---
        if (currentPage == Page::SearchName &&
            btnSubmitSrchNm.wasClicked(mousePos, mouseClickedThisFrame))
        {
            std::string name = fldNameSrch.value;
            if (name.empty())
                toast.show("Enter a name", Pal::danger);
            else
            {
                std::string out = captureOutput([&]{ searchStudentByName(name); });
                populatePanel(panel, out, Pal::textHi);
                if (out.empty()) toast.show("No student found", Pal::warning);
            }
        }

        // --- Update Grades ---
        if (currentPage == Page::UpdateGrades &&
            btnSubmitUpd.wasClicked(mousePos, mouseClickedThisFrame))
        {
            int id = fldIDUpd.toInt();
            std::string err;
            auto grades = gradeBuilderUpd.getGrades(err);

            if (id <= 0)
                toast.show("Invalid ID", Pal::danger);
            else if (!err.empty())
                toast.show(err, Pal::danger);
            else
            {
                updateStudentGrades(id, grades);
                toast.show("Grades updated!", Pal::success);
                fldIDUpd.clear();
                gradeBuilderUpd.clear();
                gradeBuilderUpd.addField();
            }
        }

        // --- View All (auto-populate when page loads) ---
        if (currentPage == Page::ViewAll && tablePanel.lines.empty())
        {
            std::string out = captureOutput([&]{ displayAllStudentsTable(); });
            populatePanel(tablePanel, out, Pal::textHi);
        }

        // --- Reports ---
        if (currentPage == Page::Reports)
        {
            auto fireReport = [&](int i)
            {
                if (reportBtns[i].wasClicked(mousePos, mouseClickedThisFrame))
                {
                    std::string out;
                    switch(i)
                    {
                        case 0: out = captureOutput([&]{ displayClassAverage();  }); break;
                        case 1: out = captureOutput([&]{ displayTopStudent();    }); break;
                        case 2: out = captureOutput([&]{ displayLowestStudent(); }); break;
                        case 3: out = captureOutput([&]{ displayPassFailSummary(); }); break;
                        case 4: out = captureOutput([&]{ displayRankedStudents(); }); break;
                    }
                    populatePanel(panel, out, Pal::accent);
                }
            };
            for (int i = 0; i < 5; i++) fireReport(i);
        }

        // ================================================================
        //  RENDER
        // ================================================================
        window.clear(Pal::bg);

        // ---- Sidebar ----
        sf::RectangleShape sidebar({SIDEBAR_W, (float)WIN_H});
        sidebar.setFillColor(Pal::sidebar);
        window.draw(sidebar);

        // Logo / title in sidebar
        auto sysTitle = makeText("SMS", font, 28, Pal::accent);
        sysTitle.setPosition(20.f, 20.f);
        window.draw(sysTitle);

        auto sysSubtitle = makeText("Student Manager", font, 11, Pal::textLow);
        sysSubtitle.setPosition(20.f, 52.f);
        window.draw(sysSubtitle);

        drawLine(window, {10.f, 74.f}, {SIDEBAR_W-10.f, 74.f}, Pal::divider);

        for (auto& btn : sideButtons) btn.draw(window, font);

        drawLine(window, {10.f, 510.f}, {SIDEBAR_W-10.f, 510.f}, Pal::divider);

        // Save + Exit at bottom
        ActionBtn btnSaveSide("Save", {15.f, 520.f, 100.f, 34.f}, Pal::accentDim);
        ActionBtn btnExitSide("Exit", {125.f, 520.f, 100.f, 34.f}, sf::Color(80,40,40));
        btnSaveSide.update(mousePos);
        btnExitSide.update(mousePos);
        btnSaveSide.draw(window, font);
        btnExitSide.draw(window, font);

        if (btnSaveSide.wasClicked(mousePos, mouseClickedThisFrame))
        { saveToFile(); toast.show("Saved!", Pal::success); }
        if (btnExitSide.wasClicked(mousePos, mouseClickedThisFrame))
        { saveToFile(); window.close(); }

        // Version tag
        auto ver = makeText("v1.0  DSA Project", font, 10, Pal::textLow);
        ver.setPosition(15.f, WIN_H - 22.f);
        window.draw(ver);

        // ---- Vertical divider line ----
        drawLine(window, {SIDEBAR_W, 0}, {SIDEBAR_W, (float)WIN_H}, Pal::divider);

        // ================================================================
        //  CONTENT AREA - page-specific rendering
        // ================================================================

        auto pageTitle = [&](const std::string& t, const std::string& sub = "")
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
            drawLine(window,
                     {CONTENT_X, CONTENT_Y + 48.f},
                     {(float)WIN_W - 10.f, CONTENT_Y + 48.f}, Pal::divider);
        };

        // ---- DASHBOARD ----
        if (currentPage == Page::Dashboard)
        {
            pageTitle("Dashboard", "Overview & Quick Actions");

            // Stats cards
            struct Card { std::string label; std::string val; sf::Color col; };
            // Gather quick stats from console captures
            std::string avgOut = captureOutput([&]{ displayClassAverage(); });
            std::string topOut = captureOutput([&]{ displayTopStudent();   });

            std::vector<Card> cards = {
                {"Quick Save", "All data", Pal::accentDim},
                {"Class Average", "See Reports", Pal::accent},
                {"Top Student",   "See Reports", Pal::success},
                {"Pass / Fail",   "See Reports", Pal::warning},
            };

            float cx = CONTENT_X + 20.f;
            float cy = CONTENT_Y + 70.f;
            float cw = 190.f, ch = 90.f, gap = 16.f;
            for (int i = 0; i < 4; i++)
            {
                float x = cx + (cw + gap) * i;
                drawRoundRect(window, {x, cy}, {cw, ch}, Pal::card, 8.f);

                sf::RectangleShape top({cw, 4.f});
                top.setPosition(x, cy);
                top.setFillColor(cards[i].col);
                window.draw(top);

                auto lbl = makeText(cards[i].label, font, 12, Pal::textMid);
                lbl.setPosition(x + 12.f, cy + 14.f);
                window.draw(lbl);

                auto val = makeText(cards[i].val, font, 13, Pal::textHi);
                val.setPosition(x + 12.f, cy + 38.f);
                window.draw(val);
            }

            // Save button
            float by = cy + ch + 30.f;
            btnSaveData.bounds = {CONTENT_X + 20.f, by, 160.f, 38.f};
            btnSaveData.draw(window, font);

            auto hint = makeText("Tip: use the sidebar to navigate between sections.",
                                  font, 12, Pal::textLow);
            hint.setPosition(CONTENT_X + 20.f, by + 58.f);
            window.draw(hint);
        }

        // ---- ADD STUDENT ----
        else if (currentPage == Page::AddStudent)
        {
            pageTitle("Add Student", "Fill in the details below");

            fldID.draw(window, font, "Student ID");
            fldName.bounds.top = 185.f;
            fldName.draw(window, font, "Full Name");

            gradeBuilderAdd.startY = 240.f;
            // update addBtn position dynamically
            float lastY = 240.f + 38.f +
                          (float)gradeBuilderAdd.fields.size() * 38.f;
            gradeBuilderAdd.addBtn.bounds.top = lastY;
            btnSubmitAdd.bounds.top = lastY + 50.f;

            gradeBuilderAdd.draw(window, font);
            btnSubmitAdd.draw(window, font);
        }

        // ---- REMOVE STUDENT ----
        else if (currentPage == Page::RemoveStudent)
        {
            pageTitle("Remove Student", "Enter the student ID to delete");

            fldIDRem.bounds = {CONTENT_X+20, 120, 220, 36};
            fldIDRem.draw(window, font, "Student ID");

            btnSubmitRem.bounds.top = 175.f;
            btnSubmitRem.draw(window, font);

            auto warn = makeText("Warning: this action cannot be undone.",
                                  font, 12, Pal::danger);
            warn.setPosition(CONTENT_X + 20.f, 225.f);
            window.draw(warn);
        }

        // ---- VIEW ALL ----
        else if (currentPage == Page::ViewAll)
        {
            pageTitle("All Students");
            // Refresh button
            ActionBtn btnRefresh("Refresh", {CONTENT_X + 20.f, CONTENT_Y + 55.f, 100.f, 30.f}, Pal::accentDim);
            btnRefresh.update(mousePos);
            btnRefresh.draw(window, font);
            if (btnRefresh.wasClicked(mousePos, mouseClickedThisFrame))
            {
                std::string out = captureOutput([&]{ displayAllStudentsTable(); });
                populatePanel(tablePanel, out, Pal::textHi);
            }

            tablePanel.bounds = {CONTENT_X, CONTENT_Y + 92.f,
                                  WIN_W - CONTENT_X - 10.f,
                                  WIN_H - CONTENT_Y - 112.f};
            tablePanel.draw(window, font);
        }

        // ---- SEARCH BY ID ----
        else if (currentPage == Page::SearchID)
        {
            pageTitle("Search by ID");
            fldIDSrch.bounds = {CONTENT_X+20, 90, 220, 36};
            fldIDSrch.draw(window, font, "Student ID");
            btnSubmitSrchID.bounds = {CONTENT_X+20, 145, 120, 36};
            btnSubmitSrchID.draw(window, font);

            panel.bounds = {CONTENT_X, 200.f,
                             WIN_W - CONTENT_X - 10.f,
                             WIN_H - 220.f};
            panel.draw(window, font);
        }

        // ---- SEARCH BY NAME ----
        else if (currentPage == Page::SearchName)
        {
            pageTitle("Search by Name");
            fldNameSrch.bounds = {CONTENT_X+20, 90, 300, 36};
            fldNameSrch.draw(window, font, "Name (partial OK)");
            btnSubmitSrchNm.bounds = {CONTENT_X+20, 145, 120, 36};
            btnSubmitSrchNm.draw(window, font);

            panel.bounds = {CONTENT_X, 200.f,
                             WIN_W - CONTENT_X - 10.f,
                             WIN_H - 220.f};
            panel.draw(window, font);
        }

        // ---- UPDATE GRADES ----
        else if (currentPage == Page::UpdateGrades)
        {
            pageTitle("Update Grades", "Replaces all existing grades");

            fldIDUpd.bounds = {CONTENT_X+20, 90, 220, 36};
            fldIDUpd.draw(window, font, "Student ID");

            gradeBuilderUpd.startY = 155.f;
            float lastY = 155.f + 38.f +
                          (float)gradeBuilderUpd.fields.size() * 38.f;
            gradeBuilderUpd.addBtn.bounds.top = lastY;
            btnSubmitUpd.bounds.top = lastY + 50.f;

            gradeBuilderUpd.draw(window, font);
            btnSubmitUpd.draw(window, font);
        }

        // ---- REPORTS ----
        else if (currentPage == Page::Reports)
        {
            pageTitle("Reports", "Click a report to view results");

            for (auto& b : reportBtns) b.draw(window, font);

            // Result area label
            drawLine(window,
                     {CONTENT_X, 340.f}, {(float)WIN_W-10.f, 340.f}, Pal::divider);
            auto rlbl = makeText("Result", font, 12, Pal::textMid);
            rlbl.setPosition(CONTENT_X+20.f, 348.f);
            window.draw(rlbl);

            panel.bounds = {CONTENT_X, 368.f,
                             WIN_W - CONTENT_X - 10.f,
                             WIN_H - 388.f};
            panel.draw(window, font);
        }

        // ---- Toast ----
        toast.draw(window, font, (float)WIN_W, (float)WIN_H);

        window.display();
    }

    return 0;
}
