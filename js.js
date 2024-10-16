// 获取法定假期长度
function getStatutoryLength(holidayCode) {
    if (holidayCode === 1 || holidayCode === 6) {
        return 3;
    } else {
        return 1;
    }
}

// 判断日期是否是周末
function isWeekend(date) {
    const weekday = date.getDay(); // 0（周日）到 6（周六）
    return weekday === 0 || weekday === 6;
}

// 检查日期是否在列表中
function isDateInList(dateStr, dates) {
    return dates.includes(dateStr);
}

// 解析日期字符串为 Date 对象
function parseDate(dateStr) {
    const parts = dateStr.split('-');
    return new Date(parts[0], parts[1] - 1, parts[2]);
}

// 格式化 Date 对象为字符串
function formatDate(date) {
    const y = date.getFullYear();
    const m = ('0' + (date.getMonth() + 1)).slice(-2);
    const d = ('0' + date.getDate()).slice(-2);
    return `${y}-${m}-${d}`;
}

// 读取输入并处理
function main(input) {
    const lines = input.trim().split('\n');

    let index = 0;

    // 读取假期代码
    const holidayCode = parseInt(lines[index++].trim());

    // 读取原始假期开始日期
    const startDateStr = lines[index++].trim();

    // 读取假期天数
    const totalHolidayCount = parseInt(lines[index++].trim());

    // 读取假期日期列表
    const totalHolidays = [];
    while (totalHolidays.length < totalHolidayCount) {
        if (index >= lines.length) break;
        const line = lines[index++].trim();
        if (line === '') continue;
        const dates = line.split(/\s+/);
        totalHolidays.push(...dates);
    }

    // 读取调休工作日数量
    const adjustedWorkdayCount = parseInt(lines[index++].trim());

    // 读取调休工作日列表
    const adjustedWorkdays = [];
    while (adjustedWorkdays.length < adjustedWorkdayCount) {
        if (index >= lines.length) break;
        const line = lines[index++].trim();
        if (line === '') continue;
        const dates = line.split(/\s+/);
        adjustedWorkdays.push(...dates);
    }

    // 获取法定假期长度
    const statutoryLength = getStatutoryLength(holidayCode);

    // 生成原始法定假期日期
    const startDate = parseDate(startDateStr);
    const statutoryHolidays = [];
    for (let i = 0; i < statutoryLength; i++) {
        const date = new Date(startDate);
        date.setDate(date.getDate() + i);
        statutoryHolidays.push(formatDate(date));
    }

    // 处理法定假期中落在周末的日期，顺延并补偿
    const adjustedHolidays = [];
    let postponeDays = 0;
    for (let i = 0; i < statutoryLength; i++) {
        const dateStr = statutoryHolidays[i];
        const date = parseDate(dateStr);
        if (isWeekend(date)) {
            // 顺延到下一个工作日
            let delta = statutoryLength + postponeDays;
            let postponedDate;
            do {
                postponedDate = new Date(startDate);
                postponedDate.setDate(postponedDate.getDate() + delta);
                delta++;
            } while (isWeekend(postponedDate));
            adjustedHolidays.push(formatDate(postponedDate));
            postponeDays++;
        } else {
            adjustedHolidays.push(dateStr);
        }
    }

    // 构建最终的假期日期列表
    const finalHolidays = adjustedHolidays;

    // 从总假期列表中排除最终假期日期和周末，得到需要映射的日期
    const remainingDates = [];
    for (const dateStr of totalHolidays) {
        if (!isDateInList(dateStr, finalHolidays)) {
            const date = parseDate(dateStr);
            if (!isWeekend(date)) {
                remainingDates.push(dateStr);
            }
        }
    }

    // 对调休工作日和剩余假期日期进行排序
    adjustedWorkdays.sort();
    remainingDates.sort();

    // 输出调休工作日和对应的假期原始日期
    const mappings = Math.min(adjustedWorkdays.length, remainingDates.length);
    for (let i = 0; i < mappings; i++) {
        console.log(`${adjustedWorkdays[i]} ${remainingDates[i]}`);
    }
}

// 读取标准输入并调用 main 函数
const readline = require('readline');

let inputData = '';

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on('line', function(line){
    inputData += line + '\n';
});

rl.on('close', function(){
    main(inputData);
});
