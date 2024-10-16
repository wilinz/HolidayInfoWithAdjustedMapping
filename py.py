import sys
from datetime import datetime, timedelta

# 获取法定假期长度
def get_statutory_length(holiday_code):
    if holiday_code in [1, 6]:
        return 3
    else:
        return 1

# 判断日期是否是周末
def is_weekend(date):
    weekday = date.isoweekday()  # 1（周一）到 7（周日）
    return weekday >= 6

# 检查日期是否在列表中
def is_date_in_list(date_str, dates):
    return date_str in dates

# 解析日期字符串为 datetime 对象
def parse_date(date_str):
    return datetime.strptime(date_str, "%Y-%m-%d")

# 格式化 datetime 对象为字符串
def format_date(date):
    return date.strftime("%Y-%m-%d")

# 读取输入并处理
def main():
    lines = sys.stdin.read().strip().split('\n')
    index = 0

    # 读取假期代码
    holiday_code = int(lines[index].strip())
    index += 1

    # 读取原始假期开始日期
    start_date_str = lines[index].strip()
    index += 1

    # 读取假期天数
    total_holiday_count = int(lines[index].strip())
    index += 1

    # 读取假期日期列表
    total_holidays = []
    while len(total_holidays) < total_holiday_count:
        if index >= len(lines):
            break
        line = lines[index].strip()
        index += 1
        if line == '':
            continue
        dates = line.strip().split()
        total_holidays.extend(dates)

    # 读取调休工作日数量
    adjusted_workday_count = int(lines[index].strip())
    index += 1

    # 读取调休工作日列表
    adjusted_workdays = []
    while len(adjusted_workdays) < adjusted_workday_count:
        if index >= len(lines):
            break
        line = lines[index].strip()
        index += 1
        if line == '':
            continue
        dates = line.strip().split()
        adjusted_workdays.extend(dates)

    # 获取法定假期长度
    statutory_length = get_statutory_length(holiday_code)

    # 生成原始法定假期日期
    start_date = parse_date(start_date_str)
    statutory_holidays = []
    for i in range(statutory_length):
        date = start_date + timedelta(days=i)
        statutory_holidays.append(format_date(date))

    # 处理法定假期中落在周末的日期，顺延并补偿
    adjusted_holidays = []
    postpone_days = 0
    for i in range(statutory_length):
        date_str = statutory_holidays[i]
        date = parse_date(date_str)
        if is_weekend(date):
            # 顺延到下一个工作日
            delta = statutory_length + postpone_days
            while True:
                postponed_date = start_date + timedelta(days=delta)
                if not is_weekend(postponed_date):
                    break
                delta += 1
            adjusted_holidays.append(format_date(postponed_date))
            postpone_days += 1
        else:
            adjusted_holidays.append(date_str)

    # 构建最终的假期日期列表
    final_holidays = adjusted_holidays

    # 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
    remaining_dates = []
    for date_str in total_holidays:
        if not is_date_in_list(date_str, final_holidays):
            date = parse_date(date_str)
            if not is_weekend(date):
                remaining_dates.append(date_str)

    # 对调休工作日和剩余假期日期进行排序
    adjusted_workdays.sort()
    remaining_dates.sort()

    # 输出调休工作日和对应的假期原始日期
    mappings = min(len(adjusted_workdays), len(remaining_dates))
    for i in range(mappings):
        print(f"{adjusted_workdays[i]} {remaining_dates[i]}")

if __name__ == "__main__":
    main()
