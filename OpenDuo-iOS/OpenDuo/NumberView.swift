//
//  NumberView.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/13.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit

class NumberView: UIView {
    class NumberLabel: UILabel {
        var line: CALayer
        
        override init(frame: CGRect) {
            line = CALayer()
            line.backgroundColor = UIColor.white.cgColor
            super.init(frame: frame)
            self.layer.addSublayer(line)
        }
        
        required init?(coder: NSCoder) {
            line = CALayer()
            line.backgroundColor = UIColor.white.cgColor
            super.init(coder: coder)
            self.layer.addSublayer(line)
        }
        
        override func layoutSubviews() {
            super.layoutSubviews()
            line.frame = CGRect(x: 0,
                                y: self.bounds.height - 2,
                                width: self.bounds.width,
                                height: 2)
        }
    }
    
    private lazy var numberLabels: [NumberLabel] = {
        var array = [NumberLabel]()
        
        for i in 0..<numberCount {
            let label = NumberLabel(frame: CGRect.zero)
            label.font = UIFont.systemFont(ofSize: 36, weight: .medium)
            label.textAlignment = .center
            label.textColor = UIColor.white
            self.addSubview(label)
            array.append(label)
        }
        
        return array
    }()
    
    private let numberCount = 4
    private var alreadyWriteIndex = 0
    
    var string: String? {
        guard alreadyWriteIndex > 0 else {
            return nil
        }
        
        var text = ""
        
        for i in 0..<alreadyWriteIndex {
            let label = numberLabels[i]
            guard let number = label.text else {
                continue
            }
            text.append(number)
        }
        
        return text
    }
    
    var isInputCompletion: Bool {
        return (alreadyWriteIndex == numberCount) ? true : false
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        _ = numberLabels
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        
        let space: CGFloat = 35
        let spaceCount: CGFloat = CGFloat(numberLabels.count) - 1
        let fieldWidth = (self.bounds.width - (spaceCount * space)) /  CGFloat(numberLabels.count)
        
        for (index, label) in numberLabels.enumerated() {
            label.frame = CGRect(x: CGFloat(index) * (fieldWidth + space),
                                 y: 0,
                                 width: fieldWidth,
                                 height: self.bounds.height)
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        _ = numberLabels
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func append(_ number: String) {
        guard alreadyWriteIndex < 4 else {
            return
        }
        
        let label = numberLabels[alreadyWriteIndex]
        label.text = number
        alreadyWriteIndex += 1
    }
    
    func deleteLast() {
        guard alreadyWriteIndex > 0 else {
            return
        }
        
        alreadyWriteIndex -= 1
        let label = numberLabels[alreadyWriteIndex]
        label.text = nil
    }
}
