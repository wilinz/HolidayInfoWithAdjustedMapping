#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>

// 获取法定假期长度
int getStatutoryLength(int holidayCode) {
    if (holidayCode == 1 || holidayCode == 6) {
        return 3;
    } else {
        return 1;
    }
}

// 解析日期字符串 "yyyy-MM-dd" 为 std::tm 结构
std::tm parseDate(const std::string& dateStr) {
    std::tm tm = {};
    sscanf(dateStr.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;  // tm_year 是从 1900 年开始的年份数
    tm.tm_mon -= 1;      // tm_mon 范围是 0-11
    return tm;
}

// 格式化 std::tm 结构为日期字符串 "yyyy-MM-dd"
std::string formatDate(const std::tm& tm) {
    char buffer[11];
    sprintf(buffer, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    return std::string(buffer);
}

// 日期加减函数
std::tm addDays(const std::tm& date, int deltaDays) {
    std::tm tm = date;
    std::time_t time = std::mktime(&tm);
    time += deltaDays * 86400;  // 一天等于 86400 秒
    std::tm* newTm = std::localtime(&time);
    return *newTm;
}

// 计算给定日期是星期几，返回 1 表示周一，7 表示周日
int calculateWeekDay(const std::tm& date) {
    std::tm tm = date;
    std::mktime(&tm);  // 标准化日期
    int weekday = tm.tm_wday;  // tm_wday 范围是 0（周日）到 6（周六）
    if (weekday == 0) {
        weekday = 7;  // 将周日转换为 7
    }
    return weekday;
}

// 判断日期是否是周末
bool isWeekend(const std::tm& date) {
    int weekday = calculateWeekDay(date);
    return (weekday == 6 || weekday == 7);
}

// 检查日期是否在列表中
bool isDateInList(const std::string& date, const std::vector<std::string>& dates) {
    return std::find(dates.begin(), dates.end(), date) != dates.end();
}

int main() {
    int holidayCode;
    std::string line;

    // 读取假期代码
    std::getline(std::cin, line);
    holidayCode = std::stoi(line);

    // 读取原始假期开始日期
    std::string startDateStr;
    std::getline(std::cin, startDateStr);

    // 读取假期天数
    int totalHolidayCount;
    std::getline(std::cin, line);
    totalHolidayCount = std::stoi(line);

    // 读取假期日期列表
    std::vector<std::string> totalHolidays;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::string dateStr;
    while (iss >> dateStr) {
        totalHolidays.push_back(dateStr);
    }

    // 读取调休工作日数量
    int adjustedWorkdayCount;
    std::getline(std::cin, line);
    adjustedWorkdayCount = std::stoi(line);

    // 读取调休工作日列表
    std::vector<std::string> adjustedWorkdays;
    std::getline(std::cin, line);
    iss.clear();
    iss.str(line);
    while (iss >> dateStr) {
        adjustedWorkdays.push_back(dateStr);
    }

    // 获取法定假期长度
    int statutoryLength = getStatutoryLength(holidayCode);

    // 生成原始法定假期日期
    std::tm startDate = parseDate(startDateStr);
    std::vector<std::string> statutoryHolidays;
    for (int i = 0; i < statutoryLength; ++i) {
        std::tm date = addDays(startDate, i);
        statutoryHolidays.push_back(formatDate(date));
    }

    // 处理法定假期中落在周末的日期，顺延并补偿
    std::vector<std::string> adjustedHolidays;
    int postponeDays = 0;
    for (int i = 0; i < statutoryLength; ++i) {
        std::tm date = parseDate(statutoryHolidays[i]);
        if (isWeekend(date)) {
            // 顺延到下一个工作日
            int delta = statutoryLength + postponeDays;
            std::tm postponedDate;
            do {
                postponedDate = addDays(startDate, delta);
                ++delta;
            } while (isWeekend(postponedDate));
            adjustedHolidays.push_back(formatDate(postponedDate));
            ++postponeDays;
        } else {
            adjustedHolidays.push_back(statutoryHolidays[i]);
        }
    }

    // 构建最终的假期日期列表
    std::vector<std::string> finalHolidays = adjustedHolidays;

    // 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
    std::vector<std::string> remainingDates;
    for (const auto& dateStr : totalHolidays) {
        if (!isDateInList(dateStr, finalHolidays)) {
            std::tm date = parseDate(dateStr);
            if (!isWeekend(date)) {
                remainingDates.push_back(dateStr);
            }
        }
    }

    // 对调休工作日和剩余假期日期进行排序
    std::sort(adjustedWorkdays.begin(), adjustedWorkdays.end());
    std::sort(remainingDates.begin(), remainingDates.end());

    // 输出调休工作日和对应的假期原始日期
    int mappings = std::min(static_cast<int>(adjustedWorkdays.size()), static_cast<int>(remainingDates.size()));
    for (int i = 0; i < mappings; ++i) {
        std::cout << adjustedWorkdays[i] << " " << remainingDates[i] << std::endl;
    }

    return 0;
}
