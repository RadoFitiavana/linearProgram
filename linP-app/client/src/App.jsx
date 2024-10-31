// import { useState } from 'react'
// import reactLogo from './assets/react.svg'
// import viteLogo from '/vite.svg'
// import './App.css'

// function App() {
//   const [count, setCount] = useState(0)

//   return (
//     <>
//       <div>
//         <a href="https://vitejs.dev" target="_blank">
//           <img src={viteLogo} className="logo" alt="Vite logo" />
//         </a>
//         <a href="https://react.dev" target="_blank">
//           <img src={reactLogo} className="logo react" alt="React logo" />
//         </a>
//       </div>
//       <h1>Vite + React</h1>
//       <div className="card">
//         <button onClick={() => setCount((count) => count + 1)}>
//           count is {count}
//         </button>
//         <p>
//           Edit <code>src/App.jsx</code> and save to test HMR
//         </p>
//       </div>
//       <p className="read-the-docs">
//         Click on the Vite and React logos to learn more
//       </p>
//     </>
//   )
// }

// export default App

import React, { useState } from 'react';
import axios from 'axios';
import { MathJaxContext } from 'better-react-mathjax';
import ProblemTypeSelect from './components/ProblemTypeSelect';
import NumberInput from './components/NumberInput';
import ConstraintsTable from './components/ConstraintsTable';
import ObjectiveFunctionTable from './components/ObjectiveFunctionTable';
import SubmitButton from './components/SubmitButton';

export default function LinearProgramForm() {
  const [numVars, setNumVars] = useState(2);
  const [numConstraints, setNumConstraints] = useState(2);
  const [problemType, setProblemType] = useState('max');
  const [constraints, setConstraints] = useState([]);
  const [objectiveFunction, setObjectiveFunction] = useState([]);

  const handleConstraintChange = (row, col, value) => {
    const updatedConstraints = [...constraints];
    updatedConstraints[row][col] = value;
    setConstraints(updatedConstraints);
  };

  const handleSubmit = async () => {
    try {
      const response = await axios.post('http://localhost:5000/solve', {
        problemType,
        numVars,
        numConstraints,
        constraints,
        objectiveFunction,
      });
      console.log(response.data.result);
    } catch (error) {
      console.error('Error solving problem:', error);
    }
  };

  return (
    <MathJaxContext>
      <div className="container mx-auto p-4">
        <h1 className="text-2xl font-bold mb-4">Linear Program Solver</h1>
        
        <ProblemTypeSelect problemType={problemType} setProblemType={setProblemType} />
        
        <NumberInput
          label="Number of Variables"
          value={numVars}
          setValue={setNumVars}
        />

        <NumberInput
          label="Number of Constraints"
          value={numConstraints}
          setValue={setNumConstraints}
        />

        <ConstraintsTable
          numVars={numVars}
          numConstraints={numConstraints}
          constraints={constraints}
          handleConstraintChange={handleConstraintChange}
        />

        <ObjectiveFunctionTable
          numVars={numVars}
          objectiveFunction={objectiveFunction}
          setObjectiveFunction={setObjectiveFunction}
        />

        <SubmitButton handleSubmit={handleSubmit} />
      </div>
    </MathJaxContext>
  );
}
