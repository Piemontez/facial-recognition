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
		const imgType  = cols[1] || ''
		const techType = cols[2] || ''
		const procType = cols[3] || ''

		const groupName = testType + '_' + imgType + '_' + techType + '_' + procType

		const cpuFreq  = parseInt(cols[4], 10)
		const trainFreqs    = parseInt(cols[5], 10)
		const predictFreqs    = parseInt(cols[6], 10)
		const VP    = parseInt(cols[7], 10)
		const FP    = parseInt(cols[8], 10)
		const FN    = parseInt(cols[9], 10)
		const VN    = parseInt(cols[10], 10)
		const trainTime = (parseFloat(trainFreqs) / parseFloat(cpuFreq)) || 0
		const predictTime = (parseFloat(predictFreqs) / parseFloat(cpuFreq)) || 0
		
		if (testsGroup[groupName] === undefined)
			testsGroup[groupName] = {
                testType,
                imgType,
                techType,
                procType,	
				qtdTests: 0,	

				trainTimes: [],	
				trainTotal: 0,
				trainMedia: null,
				trainMin: trainTime,
				trainMax: trainTime,
                trainMediana: null,

				predictTimes: [],	
				predictTotal: 0,
				predictMedia: null,
				predictMin: predictTime,
				predictMax: predictTime,
                predictMediana: null,
                
                VP: 0,
                FP: 0,
                FN: 0,
                VN: 0,

                sensibilidade: 0,
                especificidade: 0,
                precisao: 0
			}
		
		testsGroup[groupName].qtdTests++;

        //Tempos de treinamento
		testsGroup[groupName].trainTimes.push( trainTime )
		testsGroup[groupName].trainTotal += trainTime
		if (trainTime < testsGroup[groupName].trainMin)
			testsGroup[groupName].trainMin = trainTime
		if (trainTime > testsGroup[groupName].trainMax)
			testsGroup[groupName].trainMax = trainTime

        //Tempos de predição
		testsGroup[groupName].predictTimes.push( predictTime )
		testsGroup[groupName].predictTotal += predictTime
        if (predictTime < testsGroup[groupName].predictMin)
            testsGroup[groupName].predictMin = predictTime
        if (predictTime > testsGroup[groupName].predictMax)
            testsGroup[groupName].predictMax = predictTime

        testsGroup[groupName].VP += VP
        testsGroup[groupName].FP += FP
        testsGroup[groupName].FN += FN
        testsGroup[groupName].VN += VN
	});

	for (const [key, value] of Object.entries(testsGroup)) {
        //Tempos de treinamento
		value.trainMedia = value.trainTotal / value.qtdTests
        value.trainMediana = value.trainTimes[ parseInt(value.qtdTests / 2, 10) ]
        
        //Tempos de predição
		value.predictMedia = value.predictTotal / value.qtdTests
        value.predictMediana = value.predictTimes[ parseInt(value.qtdTests / 2, 10) ]
        
        /*value.VP /= value.qtdTests
        value.FP /= value.qtdTests
        value.FN /= value.qtdTests
        value.VN /= value.qtdTests*/

        value.sensibilidade  = value.VP / (value.VP + value.FN)
        value.especificidade = value.VN / (value.FP + value.VN)
        value.precisao       = (value.VP + value.VN) / (value.VP + value.FP + value.FN + value.VN)
	}

	console.log(
		Object.entries(testsGroup).map(([key, value]) => {
			const { trainTimes, predictTimes, ...rest } = value
			return rest
		})
    )
    
    let latextFile = ''
    const decimalDef = ' $ '
    const delimiter = ' & '
    const endLine = ' \\\\\n'
    const entries = Object.entries(testsGroup)
    
    //Ordena a listagem
    entries.sort(function([akey, avalue], [bkey, bvalue]) {
	  if (avalue.precisao > bvalue.precisao)
		return -1;
	  if (avalue.precisao < bvalue.precisao)
		return 1;
	  return 0;
	})

    for (const [key, value] of entries) {
        const {testType, imgType, procType, techType} = value;
        latextFile += 
            testType + delimiter +
            imgType.replace(' Tester', '') + delimiter +
            '[' + procType.trim() + '] + ' + techType.replace('OpenCV', '') + delimiter +
            decimalDef + value.VP						+ decimalDef + delimiter +
            decimalDef + value.FP						+ decimalDef + delimiter +
            decimalDef + value.FN						+ decimalDef + delimiter +
            decimalDef + value.VN			 			+ decimalDef + delimiter +
            decimalDef + value.sensibilidade.toFixed(3)	+ decimalDef + delimiter +
            decimalDef + value.especificidade.toFixed(3)+ decimalDef + delimiter +
            decimalDef + value.precisao.toFixed(3)		+ decimalDef + delimiter +
            endLine;
    }

    console.log(latextFile);
    console.log("\nTotal de testes: " + Object.keys(testsGroup).length);
}
