/**
 * ITU project 
 * Author: Natália Holková (xholko02)
 */

var user = {login: "xlogin00", password: "12345", email: "login@mail.com"};

var categories = [{id: 0, name: "Uncategorized"}, {id: 1, name: "School"}, {id: 2, name: "Work"}];

var priorities = [{0: "no-priority", 1: "low", 2: "medium", 3: "high"}]

var tasks = [
    {
        id: 0,
        name: "Buy train ticket",
        category: 0,
        description: "Buy train ticket to go home",
        timeElapsed: 0,
        timeEstimated: 1, // h
        due: new Date("2019-12-12"),
        priority: "no-priority",
        tarification: 0.0,
        status: "paused",
        finished: undefined,
    },
    {
        id: 1,
        name: "Buy presents",
        category: 0,
        description: "Buy christmas presents",
        timeElapsed: 0,
        timeEstimated: 1, // h
        due: new Date("2019-12-24"),
        priority: "low",
        tarification: 0.0,
        status: "paused",
        finished: undefined,
    },
    {
        id: 2,
        name: "Help with flyers",
        category: 0,
        description: "Help with handing out flyers",
        timeElapsed: 0,
        timeEstimated: 4, // h
        due: new Date("2019-12-21"),
        priority: "low",
        tarification: 2.2,
        status: "paused",
        finished: undefined,
    },
    {
        id: 3,
        name: "Implement a time tracker",
        category: 1,
        description: "Implement the project for ITU",
        timeElapsed: 0,
        timeEstimated: 10, // h
        due: new Date("2019-12-08"),
        priority: "medium",
        tarification: 0.0,
        status: "paused",
        finished: undefined,
    },
    {
        id: 4,
        name: "Project Practice",
        category: 1,
        description: "Finish project for project practice",
        timeElapsed: 288000,
        timeEstimated: 100, // h
        due: new Date("2019-12-19"),
        priority: "high",
        tarification: 4.0,
        status: "paused",
        finished: undefined,
    },
    {
        id: 5,
        name: "Part time job",
        category: 2,
        description: "Finish 20 hours of work this week",
        timeElapsed: 64800,
        timeEstimated: 20, // h
        due: new Date("2019-12-22"),
        priority: "high",
        tarification: 5.0,
        status: "paused",
        finished: undefined,
    },
]

var done = [
    {
        id: 0,
        name: "Stuff already done no1",
        category: 0,
        description: "1st thing I have done",
        timeElapsed: 14400,
        timeEstimated: 4, // h
        due: new Date("2019-12-12"),
        priority: "no-priority",
        tarification: 7.0,
        status: "done",
        finished: new Date("2019-12-12"),
    },
    {
        id: 1,
        name: "Stuff already done no2",
        category: 0,
        description: "2nd thing I have done",
        timeElapsed: 3600,
        timeEstimated: 1, // h
        due: new Date("2019-12-24"),
        priority: "low",
        tarification: 1.5,
        status: "done",
        finished: new Date("2019-12-24"),
    },
    {
        id: 2,
        name: "Stuff already done no3",
        category: 1,
        description: "3rd thing I have done",
        timeElapsed: 43200,
        timeEstimated: 10, // h
        due: new Date("2019-12-08"),
        priority: "medium",
        tarification: 0.0,
        status: "done",
        finished: new Date("2019-12-08"),
    },
    {
        id: 3,
        name: "Stuff already done no4",
        category: 2,
        description: "4th thing I have done",
        timeElapsed: 72000,
        timeEstimated: 20, // h
        due: new Date("2019-12-15"),
        priority: "high",
        tarification: 5.0,
        status: "done",
        finished: new Date("2019-12-15"),
    },
]

if (localStorage.getItem("storeCategories") === null)
    localStorage.setItem('storeCategories', JSON.stringify(categories));
if (localStorage.getItem("storePriorities") === null)
    localStorage.setItem('storePriorities', JSON.stringify(priorities));
if (localStorage.getItem("storeTasks") === null)
    localStorage.setItem('storeTasks', JSON.stringify(tasks));
if (localStorage.getItem("storeDone") === null)
    localStorage.setItem('storeDone', JSON.stringify(done));
if (localStorage.getItem("storeUser") === null)
    localStorage.setItem('storeUser', JSON.stringify(user));
    
