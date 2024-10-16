package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strings"
	"time"
)

// 获取法定假期长度
func getStatutoryLength(holidayCode int) int {
	if holidayCode == 1 || holidayCode == 6 {
		return 3
	}
	return 1
}

// 判断日期是否是周末
func isWeekend(date time.Time) bool {
	weekday := date.Weekday()
	return weekday == time.Saturday || weekday == time.Sunday
}

// 检查日期是否在列表中
func isDateInList(date string, dates []string) bool {
	for _, d := range dates {
		if d == date {
			return true
		}
	}
	return false
}

// 解析日期字符串为 time.Time 对象
func parseDate(dateStr string) (time.Time, error) {
	return time.Parse("2006-01-02", dateStr)
}

func main() {
	reader := bufio.NewReader(os.Stdin)

	// 读取假期代码
	var holidayCode int
	fmt.Fscanf(reader, "%d\n", &holidayCode)

	// 读取原始假期开始日期
	startDateStr, _ := reader.ReadString('\n')
	startDateStr = strings.TrimSpace(startDateStr)

	// 读取假期天数
	var totalHolidayCount int
	fmt.Fscanf(reader, "%d\n", &totalHolidayCount)

	// 读取假期日期列表
	totalHolidays := make([]string, 0, totalHolidayCount)
	for len(totalHolidays) < totalHolidayCount {
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		dates := strings.Fields(line)
		totalHolidays = append(totalHolidays, dates...)
	}

	// 读取调休工作日数量
	var adjustedWorkdayCount int
	fmt.Fscanf(reader, "%d\n", &adjustedWorkdayCount)

	// 读取调休工作日列表
	adjustedWorkdays := make([]string, 0, adjustedWorkdayCount)
	for len(adjustedWorkdays) < adjustedWorkdayCount {
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		dates := strings.Fields(line)
		adjustedWorkdays = append(adjustedWorkdays, dates...)
	}

	// 获取法定假期长度
	statutoryLength := getStatutoryLength(holidayCode)

	// 生成原始法定假期日期
	startDate, err := parseDate(startDateStr)
	if err != nil {
		fmt.Println("Invalid start date format")
		return
	}
	statutoryHolidays := []string{}
	for i := 0; i < statutoryLength; i++ {
		date := startDate.AddDate(0, 0, i)
		statutoryHolidays = append(statutoryHolidays, date.Format("2006-01-02"))
	}

	// 处理法定假期中落在周末的日期，顺延并补偿
	adjustedHolidays := []string{}
	postponeDays := 0
	for i := 0; i < statutoryLength; i++ {
		dateStr := statutoryHolidays[i]
		date, err := parseDate(dateStr)
		if err != nil {
			fmt.Printf("Invalid date format: %s\n", dateStr)
			return
		}
		if isWeekend(date) {
			// 顺延到下一个工作日
			delta := statutoryLength + postponeDays
			var postponedDate time.Time
			for {
				postponedDate = startDate.AddDate(0, 0, delta)
				if !isWeekend(postponedDate) {
					break
				}
				delta++
			}
			adjustedHolidays = append(adjustedHolidays, postponedDate.Format("2006-01-02"))
			postponeDays++
		} else {
			adjustedHolidays = append(adjustedHolidays, date.Format("2006-01-02"))
		}
	}

	// 构建最终的假期日期列表
	finalHolidays := adjustedHolidays

	// 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
	remainingDates := []string{}
	for _, dateStr := range totalHolidays {
		if !isDateInList(dateStr, finalHolidays) {
			date, err := parseDate(dateStr)
			if err != nil {
				fmt.Printf("Invalid date format: %s\n", dateStr)
				return
			}
			if !isWeekend(date) {
				remainingDates = append(remainingDates, dateStr)
			}
		}
	}

	// 对调休工作日和剩余假期日期进行排序
	sort.Strings(adjustedWorkdays)
	sort.Strings(remainingDates)

	// 输出调休工作日和对应的假期原始日期
	mappings := adjustedWorkdayCount
	if len(remainingDates) < mappings {
		mappings = len(remainingDates)
	}

	for i := 0; i < mappings; i++ {
		fmt.Printf("%s %s\n", adjustedWorkdays[i], remainingDates[i])
	}
}
