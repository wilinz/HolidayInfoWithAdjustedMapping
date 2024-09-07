# HolidayInfoWithAdjustedMapping
## 法定假日调休日期与上班日期的映射计算方法，使用 https://github.com/6tail/lunar-flutter 的库

## 在开发课表程序时，需要根据调休映射关系以此调整日程（`adjusted_workday`与`original_day`），信息结构如下：
```json
[
   {
    "holiday_name": "中秋节",
    "target": "2024-09-15",
    "holiday_dates": [
      "2024-09-15",
      "2024-09-16",
      "2024-09-17"
    ],
    "adjusted_mapping": [
      {
        "name": "中秋节",
        "target": "2024-09-17",
        "adjusted_workday": "2024-09-14",
        "original_day": "2024-09-16"
      }
    ]
  },
  {
    "holiday_name": "国庆节",
    "target": "2024-10-01",
    "holiday_dates": [
      "2024-10-01",
      "2024-10-02",
      "2024-10-03",
      "2024-10-04",
      "2024-10-05",
      "2024-10-06",
      "2024-10-07"
    ],
    "adjusted_mapping": [
      {
        "name": "国庆节",
        "target": "2024-10-01",
        "adjusted_workday": "2024-09-29",
        "original_day": "2024-10-04"
      },
      {
        "name": "国庆节",
        "target": "2024-10-01",
        "adjusted_workday": "2024-10-12",
        "original_day": "2024-10-07"
      }
    ]
  }
]
```

## 我已经初步实现算法
```dart
import 'dart:convert'; // 导入 Dart 的 JSON 编码包
import 'package:dart_extensions/dart_extensions.dart'; // 导入 dart_extensions 扩展工具包
import 'package:intl/intl.dart'; // 导入 intl 以便进行日期格式化
import 'package:lunar/lunar.dart'; // 导入 lunar，用于处理农历功能

// 定义调休工作日信息的实体类
class AdjustedWorkday {
  String name;
  final String target;
  final String adjustedWorkday;
  final String originalDay;

  AdjustedWorkday({
    required this.name,
    required this.target,
    required this.adjustedWorkday,
    required this.originalDay,
  });

  // 将 Map 转换为 AdjustedWorkday 对象的工厂构造函数
  factory AdjustedWorkday.fromMap(Map<String, String> map) {
    return AdjustedWorkday(
      name: map['name']!,
      target: map['target']!,
      adjustedWorkday: map['adjusted_workday']!,
      originalDay: map['original_day']!,
    );
  }

  // 将 AdjustedWorkday 对象转换为 Map
  Map<String, String> toJson() {
    return {
      'name': name,
      'target': target,
      'adjusted_workday': adjustedWorkday,
      'original_day': originalDay,
    };
  }
}

// 定义假期信息的实体类
class HolidayInfo {
  final String holidayName;
  final List<String> holidayDates;
  final List<AdjustedWorkday> adjustedMapping;
  final String target;

  HolidayInfo({
    required this.holidayName,
    required this.holidayDates,
    required this.adjustedMapping,
    required this.target,
  });

  // 将 Map 转换为 HolidayInfo 对象的工厂构造函数
  factory HolidayInfo.fromJson(Map<String, dynamic> map) {
    return HolidayInfo(
      holidayName: map['holiday_name'],
      target: map['target'],
      holidayDates: List<String>.from(map['holiday_dates']),
      adjustedMapping: (map['adjusted_mapping'] as List)
          .map((e) => AdjustedWorkday.fromMap(Map<String, String>.from(e)))
          .toList(),
    );
  }

  // 将 HolidayInfo 对象转换为 Map
  Map<String, dynamic> toJson() {
    return {
      'holiday_name': holidayName,
      "target": target,
      'holiday_dates': holidayDates,
      'adjusted_mapping': adjustedMapping.map((e) => e.toJson()).toList(),
    };
  }
}

// 计算假期调休关系的函数
List<AdjustedWorkday> calculateAdjustedWorkdayRelationship(
    {required List<Holiday> holidays, // 假期列表
    required String holidayName, // 假期名称
    required int Function(String holidayName) getHolidayLength}) {
  // 用于获取假期长度的函数
  final dataTimeFormat = DateFormat("yyyy-MM-dd"); // 定义日期格式化工具，将日期格式化为字符串

  // 将每个假期的日期解析成 DateTime 对象
  final holidayDays =
      holidays.map((e) => dataTimeFormat.parse(e.getDay())).toList();

  // 过滤出不包含调休的假期
  final trueHoliday = holidays.filter((e) => !e.isWork()).toList();

  // 将不包含调休的假期解析成 DateTime 对象
  final trueHolidayDays =
      trueHoliday.map((e) => dataTimeFormat.parse(e.getDay())).toList();

  // 获取不包含调休假期的开始日期
  final holidayRawStartDay =
      dataTimeFormat.parse(trueHoliday.first.getTarget());

  // 获取假期的原始长度
  final holidayRawDay = getHolidayLength(holidayName);

  // 创建一个保存假期原始天数的列表
  final holidayRawDays = <DateTime>[];

  // 根据假期长度添加每一天的日期到列表中
  for (int i = 0; i < holidayRawDay; i++) {
    holidayRawDays.add(holidayRawStartDay.add(Duration(days: i)));
  }
  // 创建假期天数的副本
  final holidayRawDaysCopy = List.from(holidayRawDays);

  var i = 1; // 用于追踪日期的增量
  for (final day in holidayRawDays) {
    // 检查当天是否是周末
    if (day.isWeekend()) {
      // 如果是周末，则向后寻找下一个非周末的工作日
      var newDay =
          holidayRawDays[holidayRawDays.length - 1].add(Duration(days: i));
      while (true) {
        // 如果新的日期也是周末，则继续加一天
        if (newDay.isWeekend()) {
          i++;
          newDay = newDay.add(Duration(days: 1));
        } else {
          break; // 找到非周末的日期，跳出循环
        }
      }
      // 将这个新找到的日期添加到假期副本列表中
      holidayRawDaysCopy.add(newDay);
      i++; // 增加日期的增量
    }
  }

  // 创建一个列表，保存原始的调休工作日
  final originalDaysByAdjustedWorkDays = <DateTime>[];
  for (final day in trueHolidayDays) {
    // 如果日期既不是周末也不在调休假期中，则将其加入调休工作日列表
    if (!day.isWeekend() && !holidayRawDaysCopy.contains(day)) {
      originalDaysByAdjustedWorkDays.add(day);
    }
  }

  // 筛选出调休后的工作日
  final adjustedWorkDays =
      holidayDays.where((day) => !trueHolidayDays.contains(day)).toList();

  // 创建一个列表，保存调休后的工作日信息
  final adjustedDaysList = <AdjustedWorkday>[];
  for (int j = 0; j < adjustedWorkDays.length; j++) {
    try {
      // 将调休后的工作日信息存入 AdjustedWorkday 对象，并加入到列表中
      final adjustedDaysMap = AdjustedWorkday(
        name: holidayName,
        target: trueHoliday.last.getTarget(),
        adjustedWorkday: dataTimeFormat.format(adjustedWorkDays[j]),
        originalDay: dataTimeFormat.format(originalDaysByAdjustedWorkDays[j]),
      );
      adjustedDaysList.add(adjustedDaysMap);
    } catch (e) {
      // 捕获异常并输出调试信息
      print(e);
      print(holidayName);
      print(trueHoliday.first.getName());
      rethrow; // 重新抛出异常
    }
  }

  // 返回调休后的工作日列表
  return adjustedDaysList;
}

List<HolidayInfo>? getHolidayInfoWithAdjustedMapping(
    List<Holiday> holidays, List<AdjustedWorkday> adjustedWorkdays) {
  final rawAdjustedWorkdays = holidays.where((holiday) => holiday.isWork());
  final actualHolidays = holidays.where((holiday) => !holiday.isWork());

  final groupedHolidays = holidays.groupBy((Holiday holiday) => holiday.getName());
  return groupedHolidays.keys.map((holidayName) {
    return HolidayInfo(
        holidayName: holidayName,
        target: actualHolidays
            .where((holiday) => holiday.getName() == holidayName)
            .first.getDay(),
        holidayDates: actualHolidays
            .where((holiday) => holiday.getName() == holidayName)
            .map((holiday) => holiday.getDay())
            .toList(),
        adjustedMapping: rawAdjustedWorkdays
            .where((workdayHoliday) => workdayHoliday.getName() == holidayName)
            .map((workdayHoliday) {
          return adjustedWorkdays
              .firstOrNullWhere((adjustedWorkday) =>
          adjustedWorkday.adjustedWorkday == workdayHoliday.getDay())!
            ..name = holidayName;
        }).toList());
  }).toList();
}

main() {
  final holidaysYear = HolidayUtil.getHolidaysByYear(2020); 
  final nationalDayAndMidAutumnFestival =
      holidaysYear.firstOrNullWhere((e) => e.getName() == "国庆中秋") != null;

  // 按假期名称分组
  final holidays = holidaysYear.groupBy((Holiday e) => getHolidayGroupName(e));
  print(holidays);

  // 计算调休工作日关系
  final result = holidays.entries
      .map((e) => calculateAdjustedWorkdayRelationship(
          holidays: e.value,
          holidayName: getHolidayGroupName(e.value.first),
          getHolidayLength: (name) =>
              getHolidayLength(name, nationalDayAndMidAutumnFestival)))
      .where((it) => it.isNotEmpty) // 过滤掉没有调休的假期
      .toList();

  final holidays1 = holidaysYear;
  // 调用新函数，将 holidays 和 result 结合，获取完整的节日信息
  final holidayInfoList = getHolidayInfoWithAdjustedMapping(
      holidays1, result.expand((e) => e).toList());

  // 输出所有节日信息
  print("输出所有节日信息");
  print(JsonEncoder.withIndent("  ").convert(holidayInfoList));

}

// 获取假期长度的函数
int getHolidayLength(String name, bool nationalDayAndMidAutumnFestival) {
  // 如果假期名称匹配"春节"或"国庆"，则返回3天假期长度
  if (RegExp("春节|国庆").hasMatch(name)) {
    // 如果是"国庆"且同时有"国庆中秋"，返回4天假期长度
    if (nationalDayAndMidAutumnFestival && name.contains("国庆")) {
      return 4;
    }
    return 3;
  }
  return 1; // 其他假期返回1天假期长度
}

// 获取假期分组名称的函数
String getHolidayGroupName(Holiday e) => e.getName().contains("国庆中秋")
    ? e.getName().replaceAll("国庆中秋", "国庆节") // 替换"国庆中秋"为"国庆节"
    : e.getName(); // 其他假期返回原名称

// 为 DateTime 扩展的一个方法，用于判断某天是否为周末
extension WeekendExt on DateTime {
  bool isWeekend() {
    return weekday == 6 || weekday == 7; // 判断当前日期是否为周六或周日
  }
}
```
