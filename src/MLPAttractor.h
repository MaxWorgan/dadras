#pragma once
#include "genann.h"
#include "ofVec2f.h"
#include "ofLog.h"
#include "ParameterInterface.h"

using namespace std;

class MLPAttractor : public ParameterInterface{
    public:
        genann *ann;

        vector<tuple<ofVec2f, vector<double>>> trainingData;

        MLPAttractor() {
        }

        virtual int getNumOutputs() const = 0;

        void setupAnn() {
            ann = genann_init(2, 1, getNumOutputs(), getNumOutputs());
        }

        void predict( float x, float y) {
            vector<ofParameter<float>> params = getParameterValues();
            assert(params.size() == ann->outputs);

            const double data[2] = {x, y};
            const double *output = genann_run(ann, data);

            vector<float> result;
            for(size_t i = 0; i < ann->outputs; ++i){
                float v = rescaleToParameter(output[i], params[i]);
                params[i].set(v);
            }
        }

        void clearTrainingData(){
            trainingData.clear();
        }

        double scaleParameter(ofParameter<float> const &p) const {
            return ofMap(p.get(), p.getMin(), p.getMax(), 0.0,1.0);
        }

        float rescaleToParameter(double const value, ofParameter<float> const &p) const{
            return ofMap(value, 0.0, 1.0, p.getMin(), p.getMax());
        }

        void capture(float x, float y){

            vector<ofParameter<float>> currentParams = getParameterValues();

            vector<double> scaledParams;
            scaledParams.resize(currentParams.size());

            transform(currentParams.begin(), currentParams.end(), scaledParams.begin(), [this](ofParameter<float>& p) {
                return scaleParameter(p);
            });

            trainingData.push_back(make_tuple(ofVec2f(x, y), scaledParams));
        }

        double train(int numTrainingCycles, float learningRate){

            if(trainingData.empty()){
                ofLog() << "no data to train";
                return 0.0;
            }

            std::vector<vector<double>> inputs;
            std::vector<vector<double>> outputs;

            // Lambda to split the original vector
            auto splitLambda = [&inputs, &outputs](const auto& tuple) {
                 // Insert elements from the vector of doubles into the single vector
                const ofVec2f& p = std::get<0>(tuple);
                inputs.push_back({p.x, p.y});

                const vector<double>& dp = std::get<1>(tuple);
                outputs.push_back(dp);

            };

            // Apply the lambda to each element of the original vector
            std::for_each(trainingData.begin(), trainingData.end(), splitLambda);

            for(int z = 0; z < numTrainingCycles; z++){
                for (size_t i = 0; i < inputs.size(); ++i){
                    auto in = inputs[i].data();
                    auto out = outputs[i].data();
                    genann_train(ann, in, out, learningRate);
                }
            }
            const double* testOutput= genann_run(ann, inputs[0].data());
            double mse = 0;
            for(size_t i = 0; i < ann->outputs; ++i){
                mse += pow(testOutput[i] - outputs[0][i], 2);
            }
            mse /= ann->outputs;
            return mse;
        }

};

