#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 返回某个月的天数
int daysInMonth(int year, int month) {
    int daysPerMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // 闰年处理
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        return 29;
    }
    return daysPerMonth[month - 1];
}

// 解析日期字符串 "yyyy-MM-dd" 为 年、月、日
void parseDate(const char* dateStr, int* year, int* month, int* day) {
    sscanf(dateStr, "%d-%d-%d", year, month, day);
}

// 格式化日期到字符串 "yyyy-MM-dd"
void formatDate(int year, int month, int day, char* dateStr) {
    sprintf(dateStr, "%04d-%02d-%02d", year, month, day);
}

// 日期加减函数
void addDays(const char* inputDate, int deltaDays, char* resultDate) {
    int year, month, day;
    // 解析输入日期
    parseDate(inputDate, &year, &month, &day);

    // 增加或减少天数
    day += deltaDays;

    // 处理天数溢出或不足
    while (day > daysInMonth(year, month)) {
        day -= daysInMonth(year, month);
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    while (day < 1) {
        month--;
        if (month < 1) {
            month = 12;
            year--;
        }
        day += daysInMonth(year, month);
    }

    // 格式化输出结果日期
    formatDate(year, month, day, resultDate);
}

// 计算给定日期是星期几，返回1代表周一，7代表周日
int calculateWeekDay(const char* date) {
    int year, month, day;

    // 解析输入的日期字符串，格式为 "yyyy-MM-dd"
    sscanf(date, "%d-%d-%d", &year, &month, &day);

    // 将一月和二月视为上一年的13月和14月
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }

    int k = year % 100;  // 年份的后两位
    int j = year / 100;  // 年份的前两位

    // Zeller公式
    int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

    // Zeller算法中, h=0代表周六, h=1代表周日, h=2代表周一，依次类推
    int weekday = (h + 5) % 7 + 1;  // 转换为标准：1代表周一，7代表周日

    return weekday;
}

// 获取法定假期长度
int getStatutoryLength(int holidayCode) {
    if (holidayCode == 1 || holidayCode == 6) {
        return 3;
    } else {
        return 1;
    }
}

// 比较日期，date1 < date2 返回负数，等于返回0，大于返回正数
int compareDates(const char* date1, const char* date2) {
    return strcmp(date1, date2);
}

// 检查日期是否在列表中
int isDateInList(const char* date, char dates[][11], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(date, dates[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// 判断日期是否是周末
int isWeekend(const char* date) {
    int weekday = calculateWeekDay(date);
    return (weekday == 6 || weekday == 7);
}

int main() {
    int holidayCode;
    char startDate[11];
    int totalHolidayCount;
    char totalHolidays[100][11];
    int adjustedWorkdayCount;
    char adjustedWorkdays[100][11];

    // 读取假期代码
    scanf("%d", &holidayCode);
    getchar(); // 吃掉换行符

    // 读取原始假期开始日期
    scanf("%s", startDate);
    getchar();

    // 读取假期天数
    int statutoryLength;
    scanf("%d", &totalHolidayCount);
    getchar();

    // 读取假期日期列表
    for (int i = 0; i < totalHolidayCount; i++) {
        scanf("%s", totalHolidays[i]);
    }
    getchar();

    // 读取调休工作日数量
    scanf("%d", &adjustedWorkdayCount);
    getchar();

    // 读取调休工作日列表
    for (int i = 0; i < adjustedWorkdayCount; i++) {
        scanf("%s", adjustedWorkdays[i]);
    }

    // 获取法定假期长度
    statutoryLength = getStatutoryLength(holidayCode);

    // 生成原始法定假期日期
    char statutoryHolidays[10][11];
    for (int i = 0; i < statutoryLength; i++) {
        addDays(startDate, i, statutoryHolidays[i]);
    }

    // 处理法定假期中落在周末的日期，顺延并补偿
    char adjustedHolidays[20][11];
    int adjustedHolidayCount = 0;
    int postponeDays = 0;
    int lastHolidayIndex = statutoryLength - 1;
    for (int i = 0; i < statutoryLength; i++) {
        if (isWeekend(statutoryHolidays[i])) {
            // 顺延
            char postponedDate[11];
            int delta = statutoryLength + postponeDays;
            do {
                addDays(startDate, delta, postponedDate);
                delta++;
            } while (isWeekend(postponedDate));
            strcpy(adjustedHolidays[adjustedHolidayCount++], postponedDate);
            postponeDays++;
        } else {
            // 不需要顺延
            strcpy(adjustedHolidays[adjustedHolidayCount++], statutoryHolidays[i]);
        }
    }

    // 将顺延的日期追加到假期列表中
    for (int i = statutoryLength; i < adjustedHolidayCount; i++) {
        strcpy(statutoryHolidays[lastHolidayIndex + i - statutoryLength + 1], adjustedHolidays[i]);
    }
    int finalHolidayCount = lastHolidayIndex + adjustedHolidayCount - statutoryLength + 1;

    // 构建最终的假期日期列表
    char finalHolidays[20][11];
    int finalHolidaysCount = 0;
    for (int i = 0; i < adjustedHolidayCount; i++) {
        strcpy(finalHolidays[finalHolidaysCount++], adjustedHolidays[i]);
    }

    // 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
    char remainingDates[100][11];
    int remainingCount = 0;
    for (int i = 0; i < totalHolidayCount; i++) {
        if (!isDateInList(totalHolidays[i], finalHolidays, finalHolidaysCount) && !isWeekend(totalHolidays[i])) {
            strcpy(remainingDates[remainingCount++], totalHolidays[i]);
        }
    }

    // 对调休工作日和剩余假期日期进行排序
    // 按日期顺序排序
    for (int i = 0; i < adjustedWorkdayCount - 1; i++) {
        for (int j = i + 1; j < adjustedWorkdayCount; j++) {
            if (compareDates(adjustedWorkdays[i], adjustedWorkdays[j]) > 0) {
                char temp[11];
                strcpy(temp, adjustedWorkdays[i]);
                strcpy(adjustedWorkdays[i], adjustedWorkdays[j]);
                strcpy(adjustedWorkdays[j], temp);
            }
        }
    }

    for (int i = 0; i < remainingCount - 1; i++) {
        for (int j = i + 1; j < remainingCount; j++) {
            if (compareDates(remainingDates[i], remainingDates[j]) > 0) {
                char temp[11];
                strcpy(temp, remainingDates[i]);
                strcpy(remainingDates[i], remainingDates[j]);
                strcpy(remainingDates[j], temp);
            }
        }
    }

    // 映射调休工作日和剩余假期日期
    for (int i = 0; i < adjustedWorkdayCount && i < remainingCount; i++) {
        printf("%s %s\n", adjustedWorkdays[i], remainingDates[i]);
    }

    return 0;
}
