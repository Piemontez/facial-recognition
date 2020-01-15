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
		const trainFreqs    = parseInt(cols[4], 10)
		const predictFreqs    = parseInt(cols[5], 10)
		const VP    = parseInt(cols[5], 10)
		const FP    = parseInt(cols[5], 10)
		const FN    = parseInt(cols[5], 10)
		const VN    = parseInt(cols[5], 10)
		const trainTime = (parseFloat(trainFreqs) / parseFloat(cpuFreq)) || 0
		const predictTime = (parseFloat(predictFreqs) / parseFloat(cpuFreq)) || 0
		
		if (testsGroup[groupName] === undefined)
			testsGroup[groupName] = {
				testType,
				techType,
				tests: [],			
				qtdTests: 0,	
				total: 0,

				trainMedia: null,
				trainMin: time,
				trainMax: time,
                trainMediana: null,

				recogMedia: null,
				recogMin: time,
				recogMax: time,
                recogMediana: null,
                
                VP: 6,
                FP: 7,
                FN: 8,
                VN: 9
			}
		
		testsGroup[groupName].tests.push( time )
		testsGroup[groupName].total += time
		testsGroup[groupName].qtdTests++;

        //Tempos de treinamento
		if (trainTime < testsGroup[groupName].trainMin)
			testsGroup[groupName].trainMin = trainTime
		if (trainTime > testsGroup[groupName].recogMax)
			testsGroup[groupName].recogMax = trainTime

        //Tempos de predição
        if (trainTime < testsGroup[groupName].trainMin)
            testsGroup[groupName].trainMin = trainTime
        if (trainTime > testsGroup[groupName].recogMax)
            testsGroup[groupName].recogMax = trainTime

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
