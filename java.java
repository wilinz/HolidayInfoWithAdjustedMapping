import java.time.LocalDate;
import java.time.DayOfWeek;
import java.time.format.DateTimeFormatter;
import java.util.*;

public class Main {
    // 获取法定假期长度
    public static int getStatutoryLength(int holidayCode) {
        if (holidayCode == 1 || holidayCode == 6) {
            return 3;
        } else {
            return 1;
        }
    }

    // 判断日期是否是周末
    public static boolean isWeekend(LocalDate date) {
        DayOfWeek day = date.getDayOfWeek();
        return day == DayOfWeek.SATURDAY || day == DayOfWeek.SUNDAY;
    }

    // 检查日期是否在列表中
    public static boolean isDateInList(String dateStr, List<String> dates) {
        return dates.contains(dateStr);
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");

        // 读取假期代码
        int holidayCode = Integer.parseInt(scanner.nextLine().trim());

        // 读取原始假期开始日期
        String startDateStr = scanner.nextLine().trim();

        // 读取假期天数
        int totalHolidayCount = Integer.parseInt(scanner.nextLine().trim());

        // 读取假期日期列表
        List<String> totalHolidays = new ArrayList<>();
        while (totalHolidays.size() < totalHolidayCount) {
            if (!scanner.hasNextLine()) break;
            String line = scanner.nextLine().trim();
            if (line.isEmpty()) continue;
            String[] dates = line.split("\\s+");
            totalHolidays.addAll(Arrays.asList(dates));
        }

        // 读取调休工作日数量
        int adjustedWorkdayCount = Integer.parseInt(scanner.nextLine().trim());

        // 读取调休工作日列表
        List<String> adjustedWorkdays = new ArrayList<>();
        while (adjustedWorkdays.size() < adjustedWorkdayCount) {
            if (!scanner.hasNextLine()) break;
            String line = scanner.nextLine().trim();
            if (line.isEmpty()) continue;
            String[] dates = line.split("\\s+");
            adjustedWorkdays.addAll(Arrays.asList(dates));
        }

        // 获取法定假期长度
        int statutoryLength = getStatutoryLength(holidayCode);

        // 生成原始法定假期日期
        LocalDate startDate = LocalDate.parse(startDateStr, formatter);
        List<String> statutoryHolidays = new ArrayList<>();
        for (int i = 0; i < statutoryLength; i++) {
            LocalDate date = startDate.plusDays(i);
            statutoryHolidays.add(date.format(formatter));
        }

        // 处理法定假期中落在周末的日期，顺延并补偿
        List<String> adjustedHolidays = new ArrayList<>();
        int postponeDays = 0;
        for (int i = 0; i < statutoryLength; i++) {
            String dateStr = statutoryHolidays.get(i);
            LocalDate date = LocalDate.parse(dateStr, formatter);
            if (isWeekend(date)) {
                // 顺延到下一个工作日
                int delta = statutoryLength + postponeDays;
                LocalDate postponedDate;
                do {
                    postponedDate = startDate.plusDays(delta);
                    delta++;
                } while (isWeekend(postponedDate));
                adjustedHolidays.add(postponedDate.format(formatter));
                postponeDays++;
            } else {
                adjustedHolidays.add(dateStr);
            }
        }

        // 构建最终的假期日期列表
        List<String> finalHolidays = adjustedHolidays;

        // 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
        List<String> remainingDates = new ArrayList<>();
        for (String dateStr : totalHolidays) {
            if (!isDateInList(dateStr, finalHolidays)) {
                LocalDate date = LocalDate.parse(dateStr, formatter);
                if (!isWeekend(date)) {
                    remainingDates.add(dateStr);
                }
            }
        }

        // 对调休工作日和剩余假期日期进行排序
        Collections.sort(adjustedWorkdays);
        Collections.sort(remainingDates);

        // 输出调休工作日和对应的假期原始日期
        int mappings = Math.min(adjustedWorkdays.size(), remainingDates.size());
        for (int i = 0; i < mappings; i++) {
            System.out.println(adjustedWorkdays.get(i) + " " + remainingDates.get(i));
        }
    }
}
