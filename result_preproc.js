const fs = require('fs')
     
const filepath = './tmp/results.csv'
//const filepath = './tmp/teste_3d/results.csv'

const exists = fs.existsSync(filepath)

if (exists) {
	const content = fs.readFileSync(filepath).toString()
	
	const lines = content.split('\n')

	let testsGroup = {};

	lines.forEach(line => {
		const cols = line.split(';')
		
		const testType = cols[0] || ''
		const techType = cols[1] || ''
		const procType = cols[2] || ''

		const groupName = testType + '_' + techType + '_' + procType

		const cpuFreq  = parseInt(cols[3], 10)
		const freqs    = parseInt(cols[4], 10)	
		const time = (parseFloat(freqs) / parseFloat(cpuFreq)) || 0 
		
		if (testsGroup[groupName] === undefined)
			testsGroup[groupName] = {
				testType,
				techType,
				tests: [],			
				qtdTests: 0,	
				total: 0,

				media: null,
				min: time,
				max: time,
				mediana: null
			}
		
		testsGroup[groupName].tests.push( time )
		testsGroup[groupName].total += time
		testsGroup[groupName].qtdTests++;

		if (time < testsGroup[groupName].min)
			testsGroup[groupName].min = time
		if (time > testsGroup[groupName].max)
			testsGroup[groupName].max = time
			
		testsGroup[groupName].mediana = time
	});

	for (const [key, value] of Object.entries(testsGroup)) {
		value.media = value.total / value.tests.length
		value.mediana = value.tests[ parseInt(value.tests.length / 2, 10) ]
	}

	console.log(
		Object.entries(testsGroup).map(([key, value]) => {
			const { tests, ...rest } = value
			return rest
		})
	)
}
